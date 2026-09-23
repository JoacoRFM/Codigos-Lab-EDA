#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

struct Point {
    int x; int y; int z;
    Point(int a = 0, int b = 0, int c = 0) : x(a), y(b), z(c) {}
};

class Octree {
private:
    Octree* children[8];
    Point* points; // std::vector<Point> points

    // bottomLeft y h definen el espacio(cubo más grande)
    Point bottomLeft;
    double h;
    int nPoints; // puntos ingresados.
    int capacity;
    bool leaf;

    void divide() {                                                                                     //    2 atras
        double nH = h / 2;                                                                              //       ______ ______ _ 
        int mX = bottomLeft.x + static_cast<int>(nH);                                                   //      /      /      / |
        int mY = bottomLeft.y + static_cast<int>(nH);                                                   //     /   6  /   7  /  |
        int mZ = bottomLeft.z + static_cast<int>(nH);                                                   //     ------ ------    |
                                                                                                        //   /   4  /   5  / | /|
        children[0] = new Octree(Point(bottomLeft.x, bottomLeft.y, bottomLeft.z), nH, capacity);        //   ------ ------   |/ |
        children[1] = new Octree(Point(mX          , bottomLeft.y, bottomLeft.z), nH, capacity);        //  |      |      |  | 3|
        children[2] = new Octree(Point(bottomLeft.x, mY          , bottomLeft.z), nH, capacity);        //  |      |      | /|  /
        children[3] = new Octree(Point(mX          , mY          , bottomLeft.z), nH, capacity);        //   ------ ------ / | /
        children[4] = new Octree(Point(bottomLeft.x, bottomLeft.y, mZ          ), nH, capacity);        //  |      |      |  |/
        children[5] = new Octree(Point(mX          , bottomLeft.y, mZ          ), nH, capacity);        //  |  0   |   1  | /
        children[6] = new Octree(Point(bottomLeft.x, mY          , mZ          ), nH, capacity);        //   ------ ------ /
        children[7] = new Octree(Point(mX          , mY          , mZ          ), nH, capacity);        //

        leaf = false;

        for (int i = 0; i < nPoints; i++) {
            int newCube = searchNewCube(points[i]);
            children[newCube]->insert(points[i]);
        }

        delete[] points;
        points = nullptr;
        nPoints = 0;
    }

    int searchNewCube(const Point& p) {
        double mX = bottomLeft.x + h / 2;
        double mY = bottomLeft.y + h / 2;
        double mZ = bottomLeft.z + h / 2;
        
        if (p.z < mZ) {
            if (p.y < mY) {
                if (p.x < mX) {
                    return 0;
                }
                else {
                    return 1;
                }
            }
            else {
                if (p.x < mX) {
                    return 2;
                }
                else {
                    return 3;
                }
            }
        }
        else {
            if (p.y < mY) {
                if (p.x < mX) {
                    return 4;
                }
                else {
                    return 5;
                }
            }
            else {
                if (p.x < mX) {
                    return 6;
                }
                else {
                    return 7;
                }
            }
        }
    }

public:
    Octree(Point B = Point(0, 0, 0), double H = 100, int C = 2) : bottomLeft(B), h(H), capacity(C), nPoints(0), leaf(1) {
        points = new Point[capacity];
        for (int i = 0; i < 8; i++) {
            children[i] = nullptr;
        }
    };

    ~Octree() {
        if (points != nullptr) {
            delete[] points;
        }
        for (int i = 0; i < 8; i++) {
            if (children[i] != nullptr) {
                delete children[i];
            }
        }
    }

    bool exist(const Point& p) {
        if (leaf) {
            for (int i = 0; i < nPoints; i++) {
                if (points[i].x == p.x && points[i].y == p.y && points[i].z == p.z) {
                    return true;
                }
            }
            return false;
        }
        int newCube = searchNewCube(p);
        return children[newCube]->exist(p);
    };

    void insert(const Point& p) {
        if (leaf) {
            if (nPoints < capacity) {
                points[nPoints] = p;
                nPoints++;
            }
            else {
                divide();
                int newCube = searchNewCube(p);
                children[newCube]->insert(p);
            }
        }
        else {
            int newCube = searchNewCube(p);
            children[newCube]->insert(p);
        }
    }

    Point find_closest(const Point& p, int radius) {
        Point closest(99999, 99999, 99999);
        double minDist = radius;
        if (leaf) {
            for (int i = 0; i < nPoints; i++) {
                double dX = points[i].x - p.x;
                double dY = points[i].y - p.y;
                double dZ = points[i].z - p.z;
                double dist = sqrt((dX * dX) + (dY * dY) + (dZ * dZ));
                if (dist <= minDist) {
                    minDist = dist;
                    closest = points[i];
                }
            }
        }
        else {
            for (int i = 0; i < 8; i++) {
                if (children[i] != nullptr) {
                    Point childClosest = children[i]->find_closest(p, static_cast<int>(minDist));
                    if (childClosest.x != 99999) {
                        double dX = childClosest.x - p.x;
                        double dY = childClosest.y - p.y;
                        double dZ = childClosest.z - p.z;
                        double dist = sqrt((dX * dX) + (dY * dY) + (dZ * dZ));
                        if (dist < minDist) {
                            minDist = dist;
                            closest = childClosest;
                        }
                    }
                }
            }
        }
        return closest;
    }
};

int main() {

    Octree tree(Point(0, 0, 0), 100.0, 2);

    tree.insert(Point(10, 10, 10));
    tree.insert(Point(20, 20, 20));
    tree.insert(Point(15, 15, 15));

    cout << (tree.exist(Point(10, 10, 10)) ? "SI" : "NO") << endl;
    cout << (tree.exist(Point(50, 50, 50)) ? "SI" : "NO") << endl;

    Point target(12, 12, 12);
    Point closest = tree.find_closest(target, 10);

    if (closest.x != 99999) {
        cout << "(" << closest.x << ", " << closest.y << ", " << closest.z << ")" << endl;
    }
    else {
        cout << "No se encontro ningun punto dentro del radio" << endl;
    }

    return 0;
}