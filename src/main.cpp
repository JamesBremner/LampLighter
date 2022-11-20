#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

class cSource;

/// @brief An edge between sources containing lamps
class cEdge
{
public:

    int source1;   // id of fuel source at one end of edge
    int source2;   // id of fuel source at other end of edge
    int lampCount; // count of lamps on edge
    int source1fueled;  // number lamps that can be fueled from source1
    int source2fueled;  // number lamps that can be fueled from source1

    cEdge();

    /// @brief get id of fuel source at other end of edge
    /// @param i id of fuel source whose 'partner is required
    /// @return id of fuel source at other end of edge
    int other(int i)
    {
        if (source1 == i)
            return source2;
        else
            return source1;
    }

    /** @brief fuel lamps on edge from source
    * @param fuelSource 
    *
    * This does nothing if edge is NOT connected to fuel source
    */
    void fuel(const cSource &fuelSource);

    /**
     * @brief Lamps that are fueled
     * 
     * @return int fueled lamp count
     * 
     * This number may be greater than the lamp count
     * when the edge is being fueled from both sources
     * and the source radii overlap
     * which happens when one or both sources
     * are reuired to fuel another edge with more lamps
     */
    int fueledCount() const
    {
        return source1fueled + source2fueled;
    }

    void display()
    {
        std::cout << source1 << " to " << source2
                  << " lamps " << lampCount
                  << " src " << source1 << " fuels " << source1fueled
                  << " src " << source2 << " fuels " << source2fueled
                  << "\n";
    }
};

/// @brief A fueling source
class cSource
{

    int myID;                            ///< source id
    std::vector<cEdge> vEdge;            ///< edges on this source
    int myRadius;                        ///< radius of reachable lamps
    static std::vector<cSource> vSource; ///< vector of sources

public:
    cSource();
    cSource(int id, cEdge &e);

    /// @brief read input
    /// @param fname filename
    static void read(const std::string &fname);

    /// @brief find source from id
    /// @param id
    /// @return reference to source
    static cSource &find(int id);

    /// @brief Fuel the lamps
    static void fuel();

    /// @brief Display source radii and total fuel
    static void totalFuel();

    /// @brief Fuel reachable lamps
    void fuelReachableLamps();

    /// @brief Single edge with unfuelled lamps
    /// @return pointer to unfuelled edge if just one, otherwise null
    cEdge *singleUnfueledEdge();

    static bool areAllLampsFueled();

    void radius( int r )
    {
        myRadius = r;
    }

    int id() const
    {
        return myID;
    }
    int radius() const
    {
        return myRadius;
    }
};

std::vector<cSource> cSource::vSource;

cEdge::cEdge()
    : source1fueled(0),
      source2fueled(0)
{
}

void cEdge::fuel(const cSource &fuelSource)
{
    if (source1 == fuelSource.id())
        if (fuelSource.radius() > source1fueled)
            source1fueled = fuelSource.radius();
    if (source2 == fuelSource.id())
        if (fuelSource.radius() > source2fueled)
            source2fueled = fuelSource.radius();
    //fueled = source1fueled + source2fueled;
}

cSource &cSource::find(int id)
{
    for (auto &s : vSource)
        if (s.id() == id)
            return s;
    throw std::runtime_error(
        "Cannot find source " + std::to_string(id));
}

cSource::cSource()
    : myRadius(0)
{
}
cSource::cSource(int id, cEdge &e)
    : myRadius(0),
      myID(id)
{
    vEdge.push_back(e);
}

/// @brief read input file
/// @param fname input filename

void cSource::read(const std::string &fname)
{
    std::ifstream inf(fname);
    int n1, n2, lamp;
    cEdge e;

    while (!inf.eof())
    {
        // read ids of sources connected by an edge with a lamp count
        inf >> e.source1 >> e.source2 >> e.lampCount;

        // echo input
        std::cout << e.source1 << " " << e.source2 << " " << e.lampCount << "\n";

        // find first source in source vector
        try
        {
            cSource &s = cSource::find(e.source1);

            // add edge to previously found source
            s.vEdge.push_back(e);
        }
        catch (...)
        {
            // new source, add to source vector
            vSource.push_back(cSource( e.source1, e ));
        }
        try
        {
            cSource &s = cSource::find(e.source2);

            // add edge to previously found source
            s.vEdge.push_back(e);
        }
        catch (...)
        {
            // new source, add to source vector
            vSource.push_back(cSource( e.source2, e ));
        }
    }
}

/// @brief Fuel all reachable lamps from source
/// @param fuelSource fuel source with set radius

void cSource::fuelReachableLamps()
{
    // std::cout << "fuelling from " << id
    //           << " radius " << radius << "\n";

    // select other sources connected to fuelling source
    for (auto &e : vEdge)
    {
        e.fuel( *this );

        // loop over edges on other source
        for (auto &oe : cSource::find(e.other(myID)).vEdge)
            oe.fuel( *this );
    }
}

cEdge *cSource::singleUnfueledEdge()
{
    cEdge *punfuelledEdge = 0;
    for (auto &e : vEdge)
    {
        if (e.lampCount > e.fueledCount())
        {
            if (punfuelledEdge)
            {
                punfuelledEdge = 0;
                break;
            }
            punfuelledEdge = &e;
        }
    }
    return punfuelledEdge;
}

/// @brief Fuel all the lamps
void cSource::fuel()
{
    // loop while lamps are being fuelled
    bool progress = true;
    while (progress)
    {
        progress = false;

        // loop over nodes
        for (auto &s : vSource)
        {
            // std::cout << s.id << "\n";

            // check that node has just one unfuelled edge
            cEdge *unfuelledEdge = s.singleUnfueledEdge();
            if (!unfuelledEdge)
                continue;

            // fuel the edge from the source at the other end of the unfuelled edge
            auto &fuelSource = cSource::find(unfuelledEdge->other(s.id()));

            // increment radius sufficient to fuel
            // the unfuelled lamps on edge
            fuelSource.radius( 
                fuelSource.radius() + 
                unfuelledEdge->lampCount - 
                unfuelledEdge->fueledCount() );

            // fuel all edges reachable from source
            fuelSource.fuelReachableLamps();

            progress = true;
        }
    }
}

void cSource::totalFuel()
{
    std::cout << "\nsource radius\n";
    int fuel = 0;
    for (auto &s : vSource)
    {
        fuel += s.radius();
        std::cout << s.id() << " r="
                  << s.radius() << "\n";
    }
    std::cout << "\ntotal fuel " << fuel << "\n";
}

bool cSource::areAllLampsFueled()
{
    for (auto &s : vSource)
        for (auto &e : s.vEdge)
        {
            std::cout << e.source1 << " to " << e.source2
                      << " lamps " << e.lampCount << " fuelled " << e.fueledCount() << " ";
            if (e.fueledCount() < e.lampCount)
            {
                std::cout << "unfueled lamp!!!\n";
                // return false;
            }
            else if (e.fueledCount() == e.lampCount)
            {
                std::cout << "OK\n";
            }
            else
                std::cout << "radii overlap\n";
        }
    return true;
}

main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "usage >lamp <fname>\n";
        exit(1);
    }
    cSource::read(argv[1]);

    cSource::fuel();
    cSource::totalFuel();
    cSource::areAllLampsFueled();
    return 0;
}