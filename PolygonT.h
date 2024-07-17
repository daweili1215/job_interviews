#include <array>
#include <string>
#include <cstring>
#include <algorithm>
#define POLYGONINOUTSTATUS(code) \
    code(UNKNOWN) code(InPolygon) code(OnPolygonEdge) code(OutsidePolygon)

enum class PolygonTestResult {
#define ENUM_ITEM(x) x,
    POLYGONINOUTSTATUS(ENUM_ITEM)
#undef ENUM_ITEM
    STATECOUNT
};
const std::array<std::string,static_cast<int>(PolygonTestResult::STATECOUNT)> _enumItemStrings = {{
    #define ITEM_STRING(x) #x,
    POLYGONINOUTSTATUS(ITEM_STRING)
    #undef ITEM_STRING
}};

template<typename POINTARRAY>
auto PrintPolygon( const POINTARRAY &polygon) ->void
{
    using POINTTYPE = typename POINTARRAY::value_type;
    std::for_each( polygon.cbegin(), polygon.cend(), [](const POINTTYPE & point ) { std::cout << point << ", ";});
}

// Potencially needed function
template< typename POINTTYPE >
auto SegmentIntersection( const POINTTYPE &seg1start, const POINTTYPE &seg1end, const POINTTYPE &seg2start, const POINTTYPE &seg2end,
                            std::vector<POINTTYPE> &intersectons)
{   // TODO: return the intersection(s) of 2 line segments defined by seg1start, seg2end and seg2start, seg2end
    // If the line segments overlapped, the 2 end points of the overlapped segment shall be returned
    
}

template< typename POINTARRAY >
class PolygonT
{
public:
    using POINTTYPE = typename POINTARRAY::value_type;
    PolygonT( POINTARRAY &points ) : _pointArray( points) {}
    virtual ~PolygonT() = default;
    auto InPolygonTest( const POINTTYPE &point ) const -> std::string
    {
        PolygonTestResult ret = PolygonTestResult::UNKNOWN;
        // TODO: implement this function from here and changed the ret to the correct status

        //////////////////////////////////////////
        std::cout << point << " in polygon ";
        PrintPolygon( _pointArray );
        std::cout << " is " << _enumItemStrings[static_cast<int>(ret)] << std::endl;
        return _enumItemStrings[static_cast<int>(ret)];
    }
    auto ClipSegments( const POINTARRAY &tobeclippedpath, POINTARRAY &clipped ) -> void
    {   // tobeclippedpath is the line segment list that adjacent points forms a line segment
        std::cout << "use ";
        PrintPolygon( _pointArray );
        std::cout << " to clip ";
        PrintPolygon(tobeclippedpath);
        // TODO: implement a function to clip segments and return the clipped segments with 'clipped'

        std::cout << std::endl << " clipped is";
        PrintPolygon(clipped);
        std::cout << std::endl;
    }
private:
    POINTARRAY &_pointArray;
};