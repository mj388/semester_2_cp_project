#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <string>
#include <filesystem>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "maze_generation.h"
#include "dfs_algorithm.h"
#include "bfs_algorithm.h"
#include "random_walk.h"

using namespace std;
using namespace sf;
namespace fs = filesystem;

static bool saveMazeImage(const vector<vector<int>>& maze,
    const string& filename,
    int cellSize = 10)
{
    int rows = maze.size();
    if (rows == 0) return false;
    int cols = maze[0].size();
    RenderTexture rt;
    if (!rt.create(cols * cellSize, rows * cellSize)) return false;
    rt.clear(Color::Black);
    RectangleShape cellShape(Vector2f(cellSize, cellSize));
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            switch (maze[y][x]) {
            case 1: cellShape.setFillColor(Color(200, 200, 200)); break;
            case 2: cellShape.setFillColor(Color(200, 200, 100)); break;
            case 3: cellShape.setFillColor(Color(200, 100, 100)); break;
            default: cellShape.setFillColor(Color(100, 100, 100)); break;
            }
            cellShape.setPosition(x * cellSize, y * cellSize);
            rt.draw(cellShape);
        }
    }
    rt.display();
    Image img = rt.getTexture().copyToImage();
    return img.saveToFile(filename);
}

enum class Screen { Welcome, MazeInput, MazeSolving, Stats };
vector<vector<int>> realMaze;

extern pair<int, int> spawn_coordinates;
extern pair<int, int> goal_coordinates;
extern DFSStats dfs_stats;
extern BFSStats bfs_stats;
extern RandomWalkStats random_stats;

class WelcomeScreen {
public:
    void draw(RenderWindow& window) {
        window.clear(Color::Black);
        Font font; font.loadFromFile("PressStart2P-Regular.ttf");
        Text text1("MOUSE MAZE SOLVER\n", font, 63);
        Text text2("press enter to start", font, 38);
        text1.setFillColor(Color::White);
        text2.setFillColor(Color(151, 151, 151));
        text1.setPosition(110, 280);
        text2.setPosition(250, 370);
        window.draw(text1);
        window.draw(text2);
        window.display();
    }
};

class MazeInputScreen {
public:
    int rows = 11, cols = 11;
    bool inputComplete = false;
private:
    string rowInput = "", colInput = "";
    bool enteringRows = true;
public:
    void handleInput(Event& event) {
        if (event.type == Event::TextEntered && isdigit(event.text.unicode)) {
            if (enteringRows) rowInput += static_cast<char>(event.text.unicode);
            else colInput += static_cast<char>(event.text.unicode);
        }
        if (event.type == Event::KeyPressed) {
            if (event.key.code == Keyboard::Right) enteringRows = !enteringRows;
            if (event.key.code == Keyboard::Enter && !rowInput.empty() && !colInput.empty()) {
                rows = stoi(rowInput);
                cols = stoi(colInput);
                if (rows < 11) {
                    rows = 11;
                }
                else if (rows % 2 == 0) {
                    rows += 1;
                }
                if (cols < 11) {
                    cols = 11;
                }
                else if (rows % 2 == 0) {
                    cols += 1;
                }
                inputComplete = true;
            }
        }
    }

    void draw(RenderWindow& window) {
        window.clear(Color::Black);
        Font font; font.loadFromFile("PressStart2P-Regular.ttf");

        Text prompt1("ENTER SIZE OF MAZE\n", font, 63);
        prompt1.setFillColor(Color::White);
        prompt1.setPosition(75, 160);

        Text prompt2("right arrow to switch", font, 38);
        prompt2.setFillColor(Color(151, 151, 151));
        prompt2.setPosition(240, 260);

        Text prompt3("enter to generate", font, 38);
        prompt3.setFillColor(Color(151, 151, 151));
        prompt3.setPosition(310, 300);

        Text rowText("ROWS: " + rowInput, font, 25);
        rowText.setFillColor(enteringRows ? Color(200, 100, 100) : Color::White);
        rowText.setPosition(200, 500);
        Text colText("COLS: " + colInput, font, 25);
        colText.setFillColor(!enteringRows ? Color(200, 100, 100) : Color::White);
        colText.setPosition(800, 500);
        window.draw(prompt1);
        window.draw(prompt2);
        window.draw(prompt3);
        window.draw(rowText);
        window.draw(colText);
        window.display();
    }
};

class Button {
public:
    RectangleShape shape;
    Text label;
    Button(const string& text, Font& font, Vector2f pos) {
        shape.setSize({ 200,50 });
        shape.setPosition(pos);
        shape.setFillColor(Color(100, 100, 200));
        label.setFont(font);
        label.setString(text);
        label.setCharacterSize(20);
        label.setFillColor(Color::White);
        label.setPosition(pos.x + 10, pos.y + 10);
    }
    bool isClicked(Vector2f pt) const { return shape.getGlobalBounds().contains(pt); }
    void draw(RenderWindow& window) const {
        window.draw(shape);
        window.draw(label);
    }
};

class MazeSolvingScreen {
public:
    vector<vector<int>> maze;
    vector<vector<int>> solvedMaze;
    vector<Button> buttons;
    Font font;
    vector<Texture> tileTextures;
    bool finishClicked = false;
    string lastAlgorithm;

    MazeSolvingScreen() {
        font.loadFromFile("PressStart2P-Regular.ttf");

        int btnWidth = 350;
        int btnHeight = 80;
        int btnX = 150;
        int btnYStart = 100;
        int btnSpacing = 107;

        vector<string> labels = { "DFS","BFS","Random","Save","Finish" };
        for (size_t i = 0; i < labels.size(); ++i) {
            Button btn(labels[i], font, Vector2f(btnX, btnYStart + i * btnSpacing));
            btn.shape.setSize(Vector2f(btnWidth, btnHeight));
            btn.label.setCharacterSize(38);
            FloatRect lb = btn.label.getLocalBounds();
            btn.label.setOrigin(lb.left + lb.width / 2, lb.top + lb.height / 2);
            btn.label.setPosition(btnX + btnWidth / 2, btnYStart + i * btnSpacing + btnHeight / 2);
            buttons.push_back(btn);
        }

        tileTextures.resize(4);
        tileTextures[0].loadFromFile("g1.png");
        tileTextures[1].loadFromFile("w.png");
        tileTextures[2].loadFromFile("g.png");
        tileTextures[3].loadFromFile("p.png");
    }

    void setMaze(const vector<vector<int>>& fullMaze, pair<int, int> center) {
        maze.clear();
        const int viewSize = 11, half = viewSize / 2;
        int R = fullMaze.size(), C = fullMaze[0].size();
        int startY = max(0, min(center.first - half, R - viewSize));
        int startX = max(0, min(center.second - half, C - viewSize));
        for (int i = 0; i < viewSize; ++i) {
            vector<int> row;
            for (int j = 0; j < viewSize; ++j)
                row.push_back(fullMaze[startY + i][startX + j]);
            maze.push_back(move(row));
        }
    }

    void draw(RenderWindow& window) {
        window.clear(Color::Black);

        for (auto& b : buttons) b.draw(window);

        int tileSize = 50;
        int offsetX = 630;
        int offsetY = 80;

        for (int i = 0; i < maze.size(); ++i) {
            for (int j = 0; j < maze[i].size(); ++j) {
                int cell = maze[i][j];
                Sprite spr(tileTextures[cell]);
                spr.setPosition(offsetX + j * tileSize, offsetY + i * tileSize);
                Vector2u sz = tileTextures[cell].getSize();
                spr.setScale(float(tileSize) / sz.x, float(tileSize) / sz.y);
                window.draw(spr);
            }
        }
        window.display();
    }

    void handleMouse(RenderWindow& window, Vector2f mousePos) {
        for (size_t i = 0; i < buttons.size(); ++i) {
            if (!buttons[i].isClicked(mousePos)) continue;
            string label = buttons[i].label.getString();
            if (label == "Finish") { finishClicked = true; return; }
            else if (label == "DFS") {
                lastAlgorithm = "DFS";
                auto frames = dfs_algorithm(realMaze);
                animate(window, frames);
                solvedMaze = frames.back().first;
            }
            else if (label == "BFS") {
                lastAlgorithm = "BFS";
                auto frames = bfs_algorithm(realMaze);
                animate(window, frames);
                solvedMaze = frames.back().first;
            }
            else if (label == "Random") {
                lastAlgorithm = "Random Walk";
                auto frames = random_walk_algorithm(realMaze);
                animate(window, frames);
                solvedMaze = frames.back().first;
            }
            else if (label == "Save") {
                if (!solvedMaze.empty() && !lastAlgorithm.empty()) {
                    fs::path dir = fs::current_path() / "solved_mazes";
                    if (!fs::exists(dir)) fs::create_directory(dir);
                    string fname = "solved_maze_" + lastAlgorithm + ".png";
                    saveMazeImage(solvedMaze, (dir / fname).string(), 8);
                }
            }
            return;
        }
    }

private:
    void animate(RenderWindow& window,
        const vector<pair<vector<vector<int>>, pair<int, int>>>& frames) {
        double sum = realMaze.size() + realMaze[0].size();
        int delay_ms = static_cast<int>(max(10.0, 351.6 - 54.6 * log(sum)));
        for (auto& snap : frames) {
            setMaze(snap.first, snap.second);
            draw(window);
            this_thread::sleep_for(chrono::milliseconds(delay_ms));
        }
    }
};

class StatsScreen {
public:
    void draw(RenderWindow& window) {
        window.clear(Color::Black);
        Font font; font.loadFromFile("PressStart2P-Regular.ttf");

        const float baseY = 660.f;
        const float maxBarH = 450.f;
        const float barW = 80.f;
        const float gap = 40.f;
        const float groupGap = 500.f;
        const float startX = 200.f;

        int dfsM = dfs_stats.totalMoves;
        int bfsM = bfs_stats.totalMoves;
        int rwM = random_stats.totalMoves;
        long long dfsT = dfs_stats.elapsedMillis;
        long long bfsT = bfs_stats.elapsedMillis;
        long long rwT = random_stats.elapsedMillis;

        float maxMoves = float(max({ dfsM, bfsM, rwM }));
        float maxTime = float(max({ int(dfsT), int(bfsT), int(rwT) }));
        float sM = maxMoves ? maxBarH / maxMoves : 1.f;
        float sT = maxTime ? maxBarH / maxTime : 1.f;

        float mX1 = startX;
        float mX2 = mX1 + barW + gap;
        float mX3 = mX2 + barW + gap;
        float tX1 = startX + groupGap;
        float tX2 = tX1 + barW + gap;
        float tX3 = tX2 + barW + gap;

        RectangleShape b1({ barW, -dfsM * sM }); b1.setPosition(mX1, baseY); b1.setFillColor(Color(100, 100, 200));  window.draw(b1);
        RectangleShape b2({ barW, -bfsM * sM }); b2.setPosition(mX2, baseY); b2.setFillColor(Color(100, 200, 100)); window.draw(b2);
        RectangleShape b3({ barW, -rwM * sM }); b3.setPosition(mX3, baseY); b3.setFillColor(Color(200, 100, 100));   window.draw(b3);

        RectangleShape b4({ barW, -dfsT * sT }); b4.setPosition(tX1, baseY); b4.setFillColor(Color(100, 100, 200));  window.draw(b4);
        RectangleShape b5({ barW, -bfsT * sT }); b5.setPosition(tX2, baseY); b5.setFillColor(Color(100, 200, 100)); window.draw(b5);
        RectangleShape b6({ barW, -rwT * sT }); b6.setPosition(tX3, baseY); b6.setFillColor(Color(200, 100, 100));   window.draw(b6);

        auto drawLabel = [&](const string& text, float x, float valueHeight) {
            Text lbl(text, font, 22);
            lbl.setFillColor(Color::White);
            float w = lbl.getLocalBounds().width, h = lbl.getLocalBounds().height;
            lbl.setPosition(x + barW * 0.5f - w * 0.5f,
                baseY - valueHeight - h - 8);
            window.draw(lbl);
            };

        drawLabel(to_string(dfsM), mX1, dfsM * sM);
        drawLabel(to_string(bfsM), mX2, bfsM * sM);
        drawLabel(to_string(rwM), mX3, rwM * sM);

        drawLabel(to_string(dfsT), tX1, dfsT * sT);
        drawLabel(to_string(bfsT), tX2, bfsT * sT);
        drawLabel(to_string(rwT), tX3, rwT * sT);

        float lx = 60.f, ly = 40.f, lg = 30.f, sz = 20.f;
        RectangleShape cb1({ sz,sz }); cb1.setPosition(lx, ly);                    cb1.setFillColor(Color(100, 100, 200));  window.draw(cb1);
        Text t1("DFS", font, 20); t1.setPosition(lx + sz + 5, ly - 2); t1.setFillColor(Color::White); window.draw(t1);
        RectangleShape cb2({ sz,sz }); cb2.setPosition(lx, ly + lg);                 cb2.setFillColor(Color(100, 200, 100)); window.draw(cb2);
        Text t2("BFS", font, 20); t2.setPosition(lx + sz + 5, ly + lg - 2); t2.setFillColor(Color::White); window.draw(t2);
        RectangleShape cb3({ sz,sz }); cb3.setPosition(lx, ly + 2 * lg);               cb3.setFillColor(Color(200, 100, 100));   window.draw(cb3);
        Text t3("Random", font, 20); t3.setPosition(lx + sz + 5, ly + 2 * lg - 2); t3.setFillColor(Color::White); window.draw(t3);

        auto drawGroupLabel = [&](float x1, float x2, const string& s) {
            Text L(s, font, 24);
            float cx = (x1 + x2) * 0.5f + barW * 0.5f;
            L.setPosition(cx - L.getLocalBounds().width * 0.5f, baseY + 12);
            L.setFillColor(Color::White);
            window.draw(L);
            };
        drawGroupLabel(mX1, mX3, "Moves");
        drawGroupLabel(tX1, tX3, "Time (ms)");

        RectangleShape ex({ 120,50 }); ex.setPosition(1100, 630); ex.setFillColor(Color(200, 100, 100)); window.draw(ex);
        Text exT("Exit", font, 24); exT.setPosition(1115, 645); exT.setFillColor(Color::White); window.draw(exT);

        window.display();
    }

    void handleMouse(RenderWindow& window, Vector2f mp) {
        if (FloatRect(1100, 630, 120, 50).contains(mp))window.close();
    }
};

int main() {
    RenderWindow window({ 1280,720 }, "Mouse Maze Solver");
    Screen current = Screen::Welcome;
    WelcomeScreen W; MazeInputScreen I; MazeSolvingScreen S; StatsScreen T;
    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) window.close();
            if (current == Screen::Welcome) {
                if (e.type == Event::KeyPressed && e.key.code == Keyboard::Enter)
                    current = Screen::MazeInput;
            }
            else if (current == Screen::MazeInput) {
                I.handleInput(e);
                if (I.inputComplete) {
                    realMaze = generate_maze(I.rows, I.cols);
                    S.setMaze(realMaze, spawn_coordinates);
                    current = Screen::MazeSolving;
                }
            }
            else if (current == Screen::MazeSolving) {
                if (e.type == Event::MouseButtonPressed) {
                    auto mp = window.mapPixelToCoords(Mouse::getPosition(window));
                    S.handleMouse(window, mp);
                }
                if (S.finishClicked) {
                    current = Screen::Stats;
                    fs::path file = fs::current_path() / "stats.txt";
                    bool writeHeader = !fs::exists(file) || fs::file_size(file) == 0;
                    ofstream ofs(file, ios::app);
                    if (writeHeader) {
                        ofs << "rows,columns,dfs_time,dfs_moves,bfs_time,bfs_moves,random_time,random_moves\n";
                    }
                    int rows = I.rows;
                    int cols = I.cols;
                    long long dfsT = dfs_stats.elapsedMillis;
                    int dfsM = dfs_stats.totalMoves;
                    long long bfsT = bfs_stats.elapsedMillis;
                    int bfsM = bfs_stats.totalMoves;
                    long long rwT = random_stats.elapsedMillis;
                    int rwM = random_stats.totalMoves;
                    ofs << rows << "," << cols << "," << dfsT << "," << dfsM << "," << bfsT << "," << bfsM << "," << rwT << "," << rwM << "\n";
                }
            }
            else if (current == Screen::Stats) {
                if (e.type == Event::MouseButtonPressed) {
                    auto mp = window.mapPixelToCoords(Mouse::getPosition(window));
                    T.handleMouse(window, mp);
                }
            }
        }
        if (current == Screen::Welcome)W.draw(window);
        else if (current == Screen::MazeInput)I.draw(window);
        else if (current == Screen::MazeSolving)S.draw(window);
        else if (current == Screen::Stats)T.draw(window);
    }

    return 0;
}
