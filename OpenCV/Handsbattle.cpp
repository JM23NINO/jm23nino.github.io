#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cstdio>

using namespace cv;
using namespace std;

// Constantes del juego
const string WEAPONS[3] = { "Quick Sword", "Double Daggers", "Great Shield" };
const string WEAPON_CODES[3] = { "sword", "daggers", "shield" };

/**
 *  Encuentra el contorno de mayor área en una lista de contornos
 */
vector<Point> findMaxContour(vector<vector<Point>>& contours) {
    double maxArea = 0;
    vector<Point> maxContour;
    for (const auto& contour : contours) {
        double area = contourArea(contour);
        if (area > maxArea) {
            maxArea = area;
            maxContour = contour;
        }
    }
    return maxContour;
}

/**
 *  Detecta el arma seleccionada basada en dedos levantados
 */
string detectWeapon(Mat frame) {
    Mat gray, blurred, thresholded;
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blurred, Size(9, 9), 0);
    threshold(blurred, thresholded, 100, 255, THRESH_BINARY_INV);

    vector<vector<Point>> contours;
    findContours(thresholded, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.empty()) return "unknown";
    vector<Point> handContour = findMaxContour(contours);
    if (handContour.empty()) return "unknown";

    vector<int> hull;
    vector<Vec4i> defects;
    convexHull(handContour, hull, false, false);

    if (hull.size() > 3) {
        convexityDefects(handContour, hull, defects);
    }

    int fingerCount = 0;
    for (const auto& defect : defects) {
        float depth = defect[3] / 256.0f;
        if (depth > 20) {
            Point start = handContour[defect[0]];
            Point end = handContour[defect[1]];
            Point farPt = handContour[defect[2]];

            double a = norm(end - start);
            double b = norm(farPt - start);
            double c = norm(farPt - end);
            double angle = acos((b * b + c * c - a * a) / (2 * b * c)) * 180 / CV_PI;

            if (angle < 90) {
                fingerCount++;
            }
        }
    }

    int totalFingers = fingerCount + 1;

    if (totalFingers == 1) return "sword";
    else if (totalFingers == 2) return "daggers";
    else if (totalFingers >= 4) return "shield";

    return "unknown";
}

/**
 * Determina el ganador de una ronda
 */
int determineWinner(const string& weapon1, const string& weapon2) {
    if (weapon1 == "unknown" || weapon2 == "unknown") return -1;
    if (weapon1 == weapon2) return 0;

    if ((weapon1 == "sword" && weapon2 == "daggers") ||
        (weapon1 == "daggers" && weapon2 == "shield") ||
        (weapon1 == "shield" && weapon2 == "sword")) {
        return 1;
    }

    return 2;
}

/**
 *  Convierte código de arma a nombre legible
 */
string weaponCodeToName(const string& weaponCode) {
    if (weaponCode == "sword") return WEAPONS[0];
    if (weaponCode == "daggers") return WEAPONS[1];
    if (weaponCode == "shield") return WEAPONS[2];
    return "Unknown";
}

/**
 *  Muestra instrucciones del juego
 */
void showInstructions() {
    printf(" HAND WAR - INSTRUCTIONS \n");
    printf("Weapon options:\n");
    printf("1.  One finger - Quick Sword (beats Double Daggers)\n");
    printf("2.  Two fingers - Double Daggers (beat Great Shield)\n");
    printf("3.  Five fingers - Great Shield (beats Quick Sword)\n\n");
    printf("First to win 3 rounds wins the game. Ready?\n\n");
}

int main() {
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        fprintf(stderr, "Error: Could not open camera.\n");
        return -1;
    }

    cap.set(CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CAP_PROP_FRAME_HEIGHT, 480);

    int p1Score = 0, p2Score = 0, round = 1;

    showInstructions();

    while (p1Score < 3 && p2Score < 3) {
        Mat frame;
        printf("\n Round %d:\n", round);

        // Player 1 turn
        printf("Player 1, get ready to show your hand in 3 seconds.\n");
        for (int i = 3; i > 0; --i) {
            printf("%d...\n", i);
            this_thread::sleep_for(chrono::seconds(1));
        }
        cap >> frame;
        if (frame.empty()) {
            fprintf(stderr, "Error: Failed to capture image for Player 1.\n");
            continue;
        }
        imshow("Player 1", frame);
        waitKey(500);
        string p1Weapon = detectWeapon(frame);
        printf("Player 1 chooses: %s\n", weaponCodeToName(p1Weapon).c_str());
        destroyWindow("Player 1");

        // Player 2 turn
        printf("\nPlayer 2, get ready to show your hand in 3 seconds.\n");
        for (int i = 3; i > 0; --i) {
            printf("%d...\n", i);
            this_thread::sleep_for(chrono::seconds(1));
        }
        cap >> frame;
        if (frame.empty()) {
            fprintf(stderr, "Error: Failed to capture image for Player 2.\n");
            continue;
        }
        imshow("Player 2", frame);
        waitKey(500);
        string p2Weapon = detectWeapon(frame);
        printf("Player 2 chooses: %s\n", weaponCodeToName(p2Weapon).c_str());
        destroyWindow("Player 2");

        int result = determineWinner(p1Weapon, p2Weapon);
        if (result == 1) {
            printf("Player 1 wins the round!\n");
            p1Score++;
        }
        else if (result == 2) {
            printf("Player 2 wins the round!\n");
            p2Score++;
        }
        else if (result == 0) {
            printf("Draw! No points awarded.\n");
        }
        else {
            printf("Could not determine winner (gesture not recognized).\n");
        }

        printf("Score -> Player 1: %d | Player 2: %d\n", p1Score, p2Score);
        round++;

        if (p1Score < 3 && p2Score < 3) {
            printf("Next round in 2 seconds...\n");
            this_thread::sleep_for(chrono::seconds(2));
        }
    }

    printf("\n!!! GAME OVER !!!\n");
    if (p1Score == 3) {
        printf("PLAYER 1 IS THE WINNER!\n");
    }
    else {
        printf("PLAYER 2 IS THE WINNER!\n");
    }
    printf("Final score: %d - %d\n", p1Score, p2Score);

    cap.release();
    return 0;
}