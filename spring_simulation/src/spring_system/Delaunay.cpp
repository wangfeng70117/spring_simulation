//#include <vector>
//#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
//
//
//#define PAI 3.1415
//class vec3 {  //向量
//public:
//    float x, y, z;
//    vec3() {}
//    vec3(float x1, float y1, float z1 = 0) { x = x1; y = y1; z = z1; }
//    vec3(Point a) { x = a.x; y = a.y; z = a.z; }
//    vec3(Point a, Point b) { x = b.x - a.x; y = b.y - a.y; z = b.z - a.z; }  //a指向b的向量
//    void init(float x1, float y1, float z1 = 0) { x = x1; y = y1; z = z1; }
//    bool operator ==(const vec3& p)
//    {
//        return fabs(x - p.x) <= 1e-6 && fabs(y - p.y) <= 1e-6 && fabs(z - p.z) <= 1e-6;
//    }
//    vec3 operator +(const vec3& p1)
//    {
//        return vec3(p1.x + x, p1.y + y, p1.z + z);
//    }
//    vec3 operator -(const vec3& p1)
//    {
//        return vec3(x - p1.x, y - p1.y, z - p1.z);
//    }
//
//
//};
//class Point {
//public:
//    float x, y, z;
//    Point() {}
//    Point(float x1, float y1, float z1 = 0) { x = x1; y = y1; z = z1; }
//    void init(float x1, float y1, float z1 = 0) { x = x1; y = y1; z = z1; }
//    bool operator ==(const Point& p)
//    {
//        return fabs(x - p.x) <= 1e-6 && fabs(y - p.y) <= 1e-6 && fabs(z - p.z) <= 1e-6;
//    }
//    Point operator +(const Point& p1)
//    {
//        return Point(p1.x + x, p1.y + y, p1.z + z);
//    }
//    Point operator -(const Point& p1)
//    {
//        return Point(x - p1.x, y - p1.y, z - p1.z);
//    }
//};
//struct Line {
//    Point p1, p2; //有向线段，p1指向p2
//    Line() {}
//    Line(Point p1, Point p2) { this->p1 = p1; this->p2 = p2; }
//    void init(Point p1, Point p2) { this->p1 = p1; this->p2 = p2; }
//    bool operator ==(const Line& l)
//    {
//        return p1 == l.p1 && p2 == l.p2;
//    }
//};
//class Gemetry
//{
//public:
//    Gemetry() {}
//    static vec3 Cross(vec3 a, vec3 b)//法线
//    {
//        float x = a.y * b.z - a.z * b.y;
//        float y = a.z * b.x - a.x * b.z;
//        float z = a.x * b.y - a.y * b.x;
//        return  vec3(x, y, z);
//    }
//    //计算距离
//    static float PointToPoint(Point pos1, Point pos2)
//    {
//        float sum = 0;
//        sum += pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2) + pow(pos2.z - pos1.z, 2);
//        return sqrt(sum);
//    }
//    static float PointToPoint(vec3 pos1, vec3 pos2)
//    {
//        float sum = 0;
//        sum += pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2) + pow(pos2.z - pos1.z, 2);
//        return sqrt(sum);
//    }
//    //向量的模
//    static float Norm(vec3 v) {
//        float sum = 0;
//        sum += pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2);
//        return sqrt(sum);
//    }
//    //点到线的距离
//    static float PointToLine(Point p, Line l)
//    {
//        vec3 ab(l.p1, l.p2);
//        vec3 ac(l.p1, p);
//        return Norm(Cross(ab, ac)) / PointToPoint(l.p1, l.p2); //d = (AB x AC)/|AB| ,|AB X AC|/2是三角形ABC的面积，这个三角形的底是|AB|，高就是C到AB的距离
//    }
//
//    //    static float angle(float x1,float y1, float x2,float y2){
//    //        float n = (x1*x2+y1*y2);
//    //        float m = sqrt(x1*x1+y1*y1)*sqrt(x2*x2+y2*y2);
//    //        return acos(n/m)*180/PAI;
//    //    }
//    static float angle3D(Point a, Point b, Point c) {
//        vec3 v1(a - b);
//        vec3 v2(c - b);
//        float x1 = v1.x;
//        float y1 = v1.y;
//        float z1 = v1.z;
//        float x2 = v2.x;
//        float y2 = v2.y;
//        float z2 = v2.z;
//        float n = (x1 * x2 + y1 * y2 + z1 * z2);
//        float m = sqrt(x1 * x1 + y1 * y1 + z1 * z1) * sqrt(x2 * x2 + y2 * y2 + z2 * z2);
//        return acos(n / m) * 180 / PAI;
//    }
//    //空间两向量夹角
//    static float angle3D(glm::vec3 n1, glm::vec3 n2)
//    {
//        
//        float n = glm::cross(n1, n2);
//        float m = sqrt(x1 * x1 + y1 * y1 + z1 * z1) * sqrt(x2 * x2 + y2 * y2 + z2 * z2);
//        return acos(n / m) * 180 / PAI;
//    }
//    static float Cross2D(vec3 a, vec3 b)
//    {
//        return a.x * b.y - b.x * a.y;
//    }
//    static bool IsRightPoint(vec3 pt, Line line)
//    {
//        vec3 v1(line.p2.x - line.p1.x, line.p2.y - line.p1.y);//p1p2
//        vec3 v2(line.p1.x - pt.x, line.p1.y - pt.y);// pp1
//        float tmp = Cross2D(v1, v2);
//        if (tmp > 0) //大于0在右边
//        {
//            return true;
//        }
//        else
//        {
//            return false;
//        }
//    }
//    static bool IsOnLine(vec3 pt, Line line)
//    {
//        vec3 v1(line.p2.x - line.p1.x, line.p2.y - line.p1.y);//p1p2
//        vec3 v2(line.p1.x - pt.x, line.p1.y - pt.y);// pp1
//        float tmp = Cross2D(v1, v2);
//        float minx, miny;
//        float maxx, maxy;
//        if (line.p1.x < line.p2.x) {
//            minx = line.p1.x;
//            maxx = line.p2.x;
//        }
//        else {
//            minx = line.p2.x;
//            maxx = line.p1.x;
//        }
//
//        if (line.p1.y < line.p2.y) {
//            miny = line.p1.y;
//            maxy = line.p2.y;
//        }
//        else {
//            miny = line.p2.y;
//            maxy = line.p1.y;
//        }
//        if (fabs(tmp) <= 1e-5 && pt.x > minx && pt.x < maxx && pt.y>miny && pt.y < maxy) //大于0在右边
//        {
//            return true;
//        }
//        else
//        {
//            return false;
//        }
//    }
//};
//struct Triangle {
//    Point p1, p2, p3;//三个点
//    Point p[3];
//    Line l1, l2, l3;//三条线
//    Line l[3];//三条线
//    Triangle() {}
//    Triangle(Point a, Point b, Point c) { init(a, b, c); }
//    bool isInTriangle(Point a)
//    {
//        bool r1 = Gemetry::IsRightPoint(a, l1);
//        bool r2 = Gemetry::IsRightPoint(a, l2);
//        bool r3 = Gemetry::IsRightPoint(a, l3);
//        if (r1 == r2 && r2 == r3)
//            return true;
//        return false;
//    }
//    int isOnTriangle(Point a)
//    {
//        bool r1 = Gemetry::IsOnLine(a, l1);
//        bool r2 = Gemetry::IsOnLine(a, l2);
//        bool r3 = Gemetry::IsOnLine(a, l3);
//        if (r1)
//            return 1;
//        if (r2)
//            return 2;
//        if (r3)
//            return 3;
//        return 0;
//    }
//
//    void init(Point a, Point b, Point c) {
//        p1 = a;
//        p2 = b;
//        p3 = c;
//        p[0] = a;
//        p[1] = b;
//        p[2] = c;
//        l1 = Line(a, b);
//        l2 = Line(b, c);
//        l3 = Line(c, a);
//        l[0] = l1;
//        l[1] = l2;
//        l[2] = l3;
//    }
//    int  containsLine(Line l)
//    {
//        if ((l.p1 == p1 && l.p2 == p2) || (l.p1 == p2 && l.p2 == p1))
//            return 1;
//        if ((l.p1 == p2 && l.p2 == p3) || (l.p1 == p3 && l.p2 == p2))
//            return 2;
//        if ((l.p1 == p1 && l.p2 == p3) || (l.p1 == p3 && l.p2 == p1))
//            return 3;
//        return 0;
//    }
//    bool operator ==(const Triangle& tri)
//    {
//        if (p1 == tri.p1 && p2 == tri.p2 && p3 == tri.p3)
//            return true;
//        return false;
//    }
//};
//class Delaunay {
//    std::vector<Triangle> ConvexHull::DivideHell(List<Point> pts)
//    {
//        QList<Point> hpts;
//        for (int i = 0; i < pts.size(); i++)
//            hpts.push_back(pts[i]);
//        tins.clear();
//        int index = 0;
//        while (hpts.size() > 2)
//        {
//            int tag = index;
//            float minangle = 180;  //每次构成相邻的边，优先找角度最小的
//            float maxangle = 0;
//
//            for (int i = index; i < hpts.size(); i++)
//            {
//                float tri_angle = 180.0;
//
//                if (i == 0) {
//                    tri_angle = Gemetry::angle3D(hpts.last(), hpts[i], hpts[i + 1]);
//
//                }
//                else if (i == hpts.size() - 1) {
//                    tri_angle = Gemetry::angle3D(hpts[i - 1], hpts[i], hpts[0]);
//                }
//                else {
//                    tri_angle = Gemetry::angle3D(hpts[i - 1], hpts[i], hpts[i + 1]);
//                }
//                if (tri_angle < minangle)
//                {
//                    tag = i;
//                    minangle = tri_angle;
//                }
//                //            tag = i;
//
//            }
//            int tagb = tag - 1;
//            int tage = tag + 1;
//            if (tag == 0)
//                tagb = hpts.size() - 1;
//            if (tag == hpts.size() - 1)
//                tage = 0;
//            tins.push_back(Triangle(hpts[tagb], hpts[tag], hpts[tage]));
//
//            hpts.removeAt(tag);
//
//
//        }
//
//        return tins;
//    }
//
//};