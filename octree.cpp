#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

struct Point {
    double x; double y; double z;
    Point(double a = 0, double b = 0, double c = 0) : x(a), y(b), z(c) {}
};

class Octree {
private:
    Octree* children[8];
    Point* points;
    Point bottomLeft;
    double h;
    int nPoints;
    int capacity;
    bool leaf;

    void divide() {
        double nH = h / 2;
        double mX = bottomLeft.x + nH;
        double mY = bottomLeft.y + nH;
        double mZ = bottomLeft.z + nH;

        children[0] = new Octree(Point(bottomLeft.x, bottomLeft.y, bottomLeft.z), nH, capacity);
        children[1] = new Octree(Point(mX, bottomLeft.y, bottomLeft.z), nH, capacity);
        children[2] = new Octree(Point(bottomLeft.x, mY, bottomLeft.z), nH, capacity);
        children[3] = new Octree(Point(mX, mY, bottomLeft.z), nH, capacity);
        children[4] = new Octree(Point(bottomLeft.x, bottomLeft.y, mZ), nH, capacity);
        children[5] = new Octree(Point(mX, bottomLeft.y, mZ), nH, capacity);
        children[6] = new Octree(Point(bottomLeft.x, mY, mZ), nH, capacity);
        children[7] = new Octree(Point(mX, mY, mZ), nH, capacity);

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
                if (p.x < mX) { return 0; }
                else { return 1; }
            }
            else {
                if (p.x < mX) { return 2; }
                else { return 3; }
            }
        }
        else {
            if (p.y < mY) {
                if (p.x < mX) { return 4; }
                else { return 5; }
            }
            else {
                if (p.x < mX) { return 6; }
                else { return 7; }
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

    Point getBottomLeft() const { return bottomLeft; }
    double getH() const { return h; }

    Octree* locateLeaf(const Point& p) {
        if (leaf) return this;
        int newCube = searchNewCube(p);
        if (children[newCube] == nullptr) return nullptr;
        return children[newCube]->locateLeaf(p);
    }
};

vector<Point> leerPuntosCSV(const string& ruta) {
    vector<Point> pts;
    ifstream file(ruta);
    if (!file.is_open()) return pts;
    string linea;
    while (getline(file, linea)) {
        if (linea.empty()) continue;
        stringstream ss(linea);
        string sx, sy, sz;
        if (!getline(ss, sx, ',')) continue;
        if (!getline(ss, sy, ',')) continue;
        if (!getline(ss, sz, ',')) continue;
        pts.push_back(Point(stod(sx), stod(sy), stod(sz)));
    }
    return pts;
}

string formatNumero(double val) {
    bool neg = val < 0;
    double av = fabs(val);
    long long parteEntera = static_cast<long long>(av);
    double frac = av - parteEntera;
    long long milesimos = static_cast<long long>(frac * 1000.0 + 1e-9);
    ostringstream oss;
    if (neg) oss << "-";
    if (milesimos == 0) {
        oss << parteEntera;
    }
    else {
        oss << parteEntera << "," << setw(3) << setfill('0') << milesimos;
    }
    return oss.str();
}

string formatPunto(const Point& p) {
    ostringstream oss;
    oss << "(" << formatNumero(p.x) << ";" << formatNumero(p.y) << ";" << formatNumero(p.z) << ")";
    return oss.str();
}

void correrConsulta(Octree& tree, Point objetivo, int radio, bool swapBlAndH = false) {
    Point cercano = tree.find_closest(objetivo, radio);
    bool noExiste = (cercano.x == 99999.0 && cercano.y == 99999.0 && cercano.z == 99999.0);

    // 1. Punto cercano
    cout << (noExiste ? "NULL" : formatPunto(cercano)) << endl;

    // Obtener hoja
    Octree* hoja = noExiste ? nullptr : tree.locateLeaf(cercano);
    string strBL = (hoja ? formatPunto(hoja->getBottomLeft()) : "NULL");
    string strH = (hoja ? formatNumero(hoja->getH()) : "NULL");

    // 2 y 3. Invertir orden si el formulario pide h antes que bottomLeft (caso consulta 6)
    if (!swapBlAndH) {
        cout << strBL << endl;
        cout << strH << endl;
    }
    else {
        cout << strH << endl;
        cout << strBL << endl;
    }
}

int main() {
    vector<Point> puntos = leerPuntosCSV("points2.csv");
    if (puntos.empty()) return 1;

    double minX = puntos[0].x, maxX = puntos[0].x;
    double minY = puntos[0].y, maxY = puntos[0].y;
    double minZ = puntos[0].z, maxZ = puntos[0].z;
    for (const auto& p : puntos) {
        minX = min(minX, p.x); maxX = max(maxX, p.x);
        minY = min(minY, p.y); maxY = max(maxY, p.y);
        minZ = min(minZ, p.z); maxZ = max(maxZ, p.z);
    }

    Point raizBottomLeft(minX, minY, minZ);
    double raizH = max({ maxX - minX, maxY - minY, maxZ - minZ });

    // Nodo raíz
    cout << formatPunto(raizBottomLeft) << endl;
    cout << formatNumero(raizH) << endl;

    Octree treeN1(raizBottomLeft, raizH, 1);
    Octree treeN5(raizBottomLeft, raizH, 5);
    Octree treeN50(raizBottomLeft, raizH, 50);

    for (const auto& p : puntos) {
        treeN1.insert(p);
        treeN5.insert(p);
        treeN50.insert(p);
    }

    // Consultas
    correrConsulta(treeN1, Point(18, 28, 175), 10);
    correrConsulta(treeN1, Point(14, -18, 116), 15);
    correrConsulta(treeN1, Point(62, 161, 305), 15);
    correrConsulta(treeN5, Point(18, -38, 128), 15);
    correrConsulta(treeN5, Point(7, -53, 135), 50);
    correrConsulta(treeN50, Point(21, 5, 143), 50, true); // Invertido h y bottomLeft para la pregunta 6

    return 0;
}