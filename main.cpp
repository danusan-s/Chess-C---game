#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "shader_s.h"
#include "chess.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr int boardSize = 8;
constexpr float squareSize = 0.8f / boardSize;

struct WindowData {
    Chessboard* chessboard;
    std::vector<int> move;
    bool black;
};

void window_size_callback(GLFWwindow* window, int width, int height) {
    int newDimension = std::min(width, height);
    glfwSetWindowSize(window, newDimension, newDimension);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    int newDimension = std::min(width, height);
    glViewport(0,0,newDimension, newDimension);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        xPos /= static_cast<double>(width);
        yPos /= static_cast<double>(height);

        xPos -= 0.1;
        yPos -= 0.1;

        int row = yPos < 0 ? -1 : yPos * 10;
        int col = xPos < 0 ? -1 : xPos * 10;

        std::cout << row << " " << col << std::endl;

        WindowData* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        if (windowData) {
            if (row >= 0 && row < 8 && col >= 0 && col < 8) {
                if (windowData->move.empty()) {
                    if (windowData->chessboard->checkValidSource(row, col, windowData->black)) {
                        windowData->move.push_back(row);
                        windowData->move.push_back(col);
                    }
                }
                else {
                    windowData->move.push_back(row);
                    windowData->move.push_back(col);
                    if (windowData->chessboard->movePiece(windowData->move, windowData->black)) {
                        windowData->black = !windowData->black;
                    }
                    windowData->move.clear();
                }
            }
            else {
                windowData->move.clear();
            }
        }
        else {
            std::cout << "Window Data not initialized" << std::endl;
        }
    }
}

void generateChessboard(std::vector<float>& vertices, std::vector<float>& colors) {

    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            float x = j * squareSize + 0.1f;
            float y = i * squareSize + 0.1f;

            // Define vertices for two triangles making up a square
            vertices.insert(vertices.end(), {
                x, y,
                x + squareSize, y,
                x, y + squareSize,
                x + squareSize, y,
                x + squareSize, y + squareSize,
                x, y + squareSize
                });

            // Define colors for the square
            bool isWhite = (i + j) % 2 == 0;
            for (int k = 0; k < 6; ++k) {
                if (isWhite) {
                    colors.insert(colors.end(), { 0.92f, 0.91f, 0.82f });
                }
                else {
                    colors.insert(colors.end(), { 0.3f, 0.45f, 0.6f });
                }
            }
        }
    }
}

void renderPiece(int row, int col, Chessboard& Game, std::vector<unsigned int>& textures,Shader& texShader) {
    float x = col * squareSize + 0.1f;
    float y =  0.8f - row * squareSize; // funky stuff convert bot-left to top-left

    Piece curr = Game.getPiece(row, col);
    if (curr.getType() == Type::None) return;
    int index = 0;
    switch (curr.getType()) {
    case Type::King:
        index = 0;
        break;
    case Type::Queen:
        index = 1;
        break;
    case Type::Rook:
        index = 2;
        break;
    case Type::Knight:
        index = 3;
        break;
    case Type::Bishop:
        index = 4;
        break;
    case Type::Pawn:
        index = 5;
        break;
    }
    if (curr.getColor() == Color::Black) index += 6;
    unsigned int textureId = textures[index];
    float vertices[] = {
        x, y,
        x + squareSize, y,
        x, y + squareSize,
        x + squareSize, y,
        x + squareSize, y + squareSize,
        x, y + squareSize
    };
    float texCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    
    unsigned int VAO, VBO[2];
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO);

    // Bind vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Bind texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // Use the texture shader program
    texShader.use();

    glBindTexture(GL_TEXTURE_2D, textureId);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glDeleteBuffers(2, VBO);
}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip loading of images vertically
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, STBI_rgb_alpha);
    if (data) {

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Generate Mipmap
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }

    return textureID;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 800, "Chessboard", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set viewport
    glViewport(0, 0, 800, 800);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    std::vector<float> vertices;
    std::vector<float> colors;
    std::vector<unsigned int> textures(12);
    generateChessboard(vertices, colors);

    // Create VBOs and VAO
    unsigned int VBO[2], VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    Chessboard Game;
    Shader OurShader("vshader.glsl", "fshader.glsl");
    Shader TexShader("tvshader.glsl", "tfshader.glsl");

    textures[0] = loadTexture("images/king_w.png");
    textures[1] = loadTexture("images/queen_w.png");
    textures[2] = loadTexture("images/rook_w.png");
    textures[3] = loadTexture("images/knight_w.png");
    textures[4] = loadTexture("images/bishop_w.png");
    textures[5] = loadTexture("images/pawn_w.png");
    textures[6] = loadTexture("images/king_b.png");
    textures[7] = loadTexture("images/queen_b.png");
    textures[8] = loadTexture("images/rook_b.png");
    textures[9] = loadTexture("images/knight_b.png");
    textures[10] = loadTexture("images/bishop_b.png");
    textures[11] = loadTexture("images/pawn_b.png");

    WindowData windowData = { &Game,{},false };
    glfwSetWindowUserPointer(window, &windowData);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render the chessboard
        OurShader.use();

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size()/2);
        glBindVertexArray(0);

        for (int row = 0; row < boardSize; ++row) {
            for (int col = 0; col < boardSize; ++col) {
                renderPiece(row, col, Game,textures,TexShader);
            }
        }

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(2, VBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
