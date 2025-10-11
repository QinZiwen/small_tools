#include <iostream>
#include <string>

class CBase {
  public:
    int m_Public;

  protected:
    int m_Protected;
    // private:
    int m_Private;
};

class CDerived : public CBase {
  public:
    void AccessBase() {
        m_Public    = 1;
        m_Protected = 2;
        // m_private = 3;

        CBase base;
        base.m_Public = 1;
        // base.m_Protected = 2;    // Cannot access directly
    }
};

struct Coordinate {
    double lon;
    double lat;
};

struct RoutePointInfo {
    int linkIndex;
    Coordinate point;
    Coordinate displayPoint;
    std::string pointName;
    int pointType;
    bool chargingAtWaypoint;

    void printVal() {
        std::cout << "linkIndex: " << linkIndex << std::endl
                  << "point: " << point.lat << " " << point.lon << std::endl
                  << "displayPoint: " << displayPoint.lat << " " << displayPoint.lon << std::endl
                  << "pointName: " << pointName << std::endl
                  << "pointType: " << pointType << std::endl
                  << "chargingAtWaypoint: " << chargingAtWaypoint << std::endl;
    }
};

int main() {
    CBase base;
    base.m_Public = 1;
    // base.m_Protected = 2;    // Cannot access directly

    CBase* pBase    = new CBase;
    pBase->m_Public = 1;
    // pBase->m_Protected = 2;    // Cannot access directly
    delete pBase;

    RoutePointInfo info;
    info.printVal();

    return 0;
}