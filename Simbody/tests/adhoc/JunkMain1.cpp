#include "SimTKsimbody.h"
#include "SimTKcommon/Testing.h"

#include <cstdio>
#include <iostream>
#include <exception>

using namespace SimTK;
using std::cout;using std::endl;


static const Real smallRad = 5*1.25/100;
static const Real largeRad = 10.5/100;
static const MobodIndex smallBodx = MobodIndex(1);
static const MobodIndex largeBodx = MobodIndex(0);
static const Vec3 smallPos(-.28,0,0);
static const Vec3 largePos(-.28,0,0);
static const Real hSmall = .03; // 6cm=100% strain
static const Real hLarge = .03;

class ForceReporter : public PeriodicEventReporter {
public:
    ForceReporter(const MultibodySystem& system, 
                  const CompliantContactSubsystem& complCont,
                  Real reportInterval)
    :   PeriodicEventReporter(reportInterval), m_system(system),
        m_compliant(complCont)
    {}

    ~ForceReporter() {}

    void handleEvent(const State& state) const {
        m_system.realize(state, Stage::Dynamics);
        //cout << state.getTime() << ": E = " << m_system.calcEnergy(state)
        //     << " Ediss=" << m_compliant.getDissipatedEnergy(state)
        //     << " E+Ediss=" << m_system.calcEnergy(state)
        //                       +m_compliant.getDissipatedEnergy(state)
        //     << endl;
        const int ncont = m_compliant.getNumContactForces(state);
        //cout << "Num contacts: " << m_compliant.getNumContactForces(state) << endl;
        if (ncont==0)
            printf("%g 0.0 0.0 0.0\n", state.getTime());

        const SimbodyMatterSubsystem& matter = m_system.getMatterSubsystem();
        const MobilizedBody& smallBod=matter.getMobilizedBody(smallBodx);
        const MobilizedBody& largeBod=matter.getMobilizedBody(largeBodx);
        const Vec3 smallCtr = smallBod.findStationLocationInGround(state, smallPos);
        const Vec3 largeCtr = largeBod.findStationLocationInGround(state, largePos);
        const Real d = (smallRad+largeRad)-(smallCtr-largeCtr).norm();
        
        for (int i=0; i < ncont; ++i) {
            const ContactForce& force = m_compliant.getContactForce(state,i);
            const ContactId     id    = force.getContactId();
            cout << state.getTime() 
                 << " " << force.getForceOnSurface2()[1][1] // Normal
                 << " " << force.getForceOnSurface2()[1][0] // Tangential
                 << " " << d << "\n"; // penetration distance

            //ContactPatch patch;
            //const bool found = m_compliant.calcContactPatchDetailsById(state,id,patch);
            //cout << "patch for id" << id << " found=" << found << endl;
            //cout << "resultant=" << patch.getContactForce() << endl;
            //cout << "num details=" << patch.getNumDetails() << endl;
            //Real patchArea=0, maxDepth=0, maxf=0;
            //Vec3 ftot(0);
            //for (int i=0; i < patch.getNumDetails(); ++i) {
            //    const ContactDetail& detail = patch.getContactDetail(i);
            //    patchArea += detail.getPatchArea();
            //    maxDepth = std::max(maxDepth, detail.getDeformation());
            //    ftot += detail.getForceOnSurface2();
            //    maxf = std::max(maxf, detail.getForceOnSurface2().norm());
            //}
            //cout << "patchArea=" << patchArea 
            //     << " ftot=" << ftot << " maxDepth=" << maxDepth << "\n";
            //cout << "maxf=" << maxf << "\n";
        }
    }
private:
    const MultibodySystem&           m_system;
    const CompliantContactSubsystem& m_compliant;
};

// Nylon
static const Real nylon_density = 1100.;  // kg/m^3
static const Real nylon_young   = .05*2.5e9;  // pascals (N/m)
static const Real nylon_poisson = 0.4;    // ratio
static const Real nylon_planestrain = 
    ContactMaterial::calcPlaneStrainStiffness(nylon_young, nylon_poisson);
static const Real nylon_confined =
    ContactMaterial::calcConfinedCompressionStiffness(nylon_young, nylon_poisson);
static const Real nylon_dissipation = 10*0.005;

static void makeSphere(Real radius, int level, PolygonalMesh& sphere);

int main() {
  try
  { // Create the system.
    
    MultibodySystem         system; system.setUseUniformBackground(true);
    SimbodyMatterSubsystem  matter(system);
    GeneralForceSubsystem   forces(system);
    Force::Gravity          gravity(forces, matter, -YAxis, 10);
    //Force::GlobalDamper     damper(forces, matter, 1.0);

    ContactTrackerSubsystem  tracker(system);
    CompliantContactSubsystem contactForces(system, tracker);
    contactForces.setTransitionVelocity(1e-3);

    // g=10, mass==.5 => weight = .5*10=5N.
    // body origin at the hinge on the right
    Real mass = 0.5;
    Vec3 com = Vec3(-.14,0,0);
    Vec3 halfDims(.14, .01, .01); // a rectangular solid
    Inertia inertia = mass*
        UnitInertia::brick(halfDims).shiftFromCentroid(Vec3(.14,0,0));

    Body::Rigid lidBody(MassProperties(mass,com,inertia));
    lidBody.addDecoration(Vec3(-.14,0,0), 
        DecorativeBrick(halfDims).setColor(Cyan).setOpacity(.1));

    PolygonalMesh smallMesh, largeMesh;

#define USE_VIZ
//#define SHOW_NORMALS
Array_<DecorativeLine> smallNormals;
Array_<DecorativeLine> largeNormals;
const Real NormalLength = .001;

#define USE_MESH_SMALL
#define USE_MESH_BIG
#ifdef USE_MESH_SMALL
    makeSphere(smallRad, 5, smallMesh);
    std::cerr << "small mesh faces: " << smallMesh.getNumFaces() << "\n";
    ContactGeometry::TriangleMesh smallContact(smallMesh);
    DecorativeMesh smallArtwork(smallContact.createPolygonalMesh());
    smallArtwork.setColor(Red);
    #ifdef SHOW_NORMALS
    for (int fx=0; fx < smallContact.getNumFaces(); ++fx) {
        smallNormals.push_back(
        DecorativeLine(smallContact.findCentroid(fx),
                       smallContact.findCentroid(fx)
                           + NormalLength*smallContact.getFaceNormal(fx))
                           .setColor(Black).setLineThickness(2));
    }
    #endif
#else
    DecorativeSphere smallArtwork(smallRad);
    smallArtwork.setResolution(2).setRepresentation(DecorativeGeometry::DrawWireframe);
    ContactGeometry::Sphere smallContact(smallRad);
#endif
#ifdef USE_MESH_BIG
    makeSphere(largeRad, 6, largeMesh);
    std::cerr << "large mesh faces: " << largeMesh.getNumFaces() << "\n";
    ContactGeometry::TriangleMesh largeContact(largeMesh);
    DecorativeMesh largeArtwork(largeContact.createPolygonalMesh());
    //DecorativeSphere largeArtwork(largeRad);
    largeArtwork.setColor(Green);
    #ifdef SHOW_NORMALS

    for (int fx=0; fx < largeContact.getNumFaces(); ++fx) {
        largeNormals.push_back(
        DecorativeLine(largeContact.findCentroid(fx),
                       largeContact.findCentroid(fx)
                           + NormalLength*largeContact.getFaceNormal(fx))
                           .setColor(Black).setLineThickness(2));
    }
    #endif
#else
    DecorativeSphere largeArtwork(largeRad);
    largeArtwork.setResolution(4).setRepresentation(DecorativeGeometry::DrawWireframe);
    ContactGeometry::Sphere largeContact(largeRad);
#endif


    ContactMaterial nylon(
        //nylon_planestrain, 
        nylon_confined, 
        nylon_dissipation, 
        .1*.9,.1*.8,.1*.6); // static, dynamic, viscous friction

    lidBody.addDecoration(smallPos, 
        smallArtwork.setOpacity(0.5));
    lidBody.addDecoration(smallPos, 
        smallArtwork.setRepresentation(DecorativeGeometry::DrawWireframe));

    for (unsigned i=0; i < smallNormals.size(); ++i)
        lidBody.addDecoration(smallPos, smallNormals[i]);

    lidBody.addContactSurface(smallPos, 
        ContactSurface(smallContact, nylon, hSmall));

    matter.Ground().updBody().addDecoration(largePos, 
        largeArtwork.setOpacity(0.5));
    matter.Ground().updBody().addDecoration(largePos, 
        largeArtwork.setRepresentation(DecorativeGeometry::DrawWireframe));

    for (unsigned i=0; i < largeNormals.size(); ++i)
        matter.Ground().updBody().addDecoration(largePos, 
        largeNormals[i]);
    matter.Ground().updBody().addContactSurface(largePos, 
        ContactSurface(largeContact, nylon, hLarge));

    MobilizedBody::Pin lid(matter.Ground(), Vec3(0.05,smallRad+largeRad,0), 
                           lidBody,         Vec3(0,0,0));

#ifdef USE_VIZ
    Visualizer viz(system);
    viz.setCameraClippingPlanes(.01,100);
#endif

    system.addEventReporter(new Visualizer::Reporter(viz, 1./1000));
    ForceReporter* frcReporter = 
        new ForceReporter(system, contactForces, 1./100000);
    system.addEventReporter(frcReporter);

    RungeKuttaMersonIntegrator integ(system);
    //CPodesIntegrator integ(system);
    //integ.setMaximumStepSize(.01);
    integ.setAccuracy(1e-5);
    TimeStepper ts(system, integ);

    system.realizeTopology();
    State startState = system.getDefaultState();
    lid.setOneQ(startState, 0, -Pi/10);
    //lid.setOneQ(startState, 0, 0.0610351);
    system.realize(startState, Stage::Position);
#ifdef USE_VIZ
    viz.report(startState);
#endif
    system.realize(startState);
    //frcReporter->handleEvent(startState);
    //printf("type to go ...\n");
   // getchar();
    ts.initialize(startState);
    ts.stepTo(0.5);

  } catch (const std::exception& e) {
    std::printf("EXCEPTION THROWN: %s\n", e.what());
    exit(1);

  } catch (...) {
    std::printf("UNKNOWN EXCEPTION THROWN\n");
    exit(1);
  }

    return 0;
}



static void makeOctahedralMesh(const Vec3& r, Array_<Vec3>& vertices,
                               Array_<int>&  faceIndices) {
    vertices.push_back(Vec3( r[0],  0,  0));   //0
    vertices.push_back(Vec3(-r[0],  0,  0));   //1
    vertices.push_back(Vec3( 0,  r[1],  0));   //2
    vertices.push_back(Vec3( 0, -r[1],  0));   //3
    vertices.push_back(Vec3( 0,  0,  r[2]));   //4
    vertices.push_back(Vec3( 0,  0, -r[2]));   //5
    int faces[8][3] = {{0, 2, 4}, {4, 2, 1}, {1, 2, 5}, {5, 2, 0}, 
                       {4, 3, 0}, {1, 3, 4}, {5, 3, 1}, {0, 3, 5}};
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 3; j++)
            faceIndices.push_back(faces[i][j]);
}

// Create a triangle mesh in the shape of an octahedron (like two 
// pyramids stacked base-to-base, with the square base in the x-z plane 
// centered at 0,0,0 of given "radius" r. 
// The apexes will be at (0,+/-r,0).
static void makeOctahedron(Real r, PolygonalMesh& mesh) {
    Array_<Vec3> vertices;
    Array_<int> faceIndices;
    makeOctahedralMesh(Vec3(r), vertices, faceIndices);

    for (unsigned i=0; i < vertices.size(); ++i)
        mesh.addVertex(vertices[i]);
    for (unsigned i=0; i < faceIndices.size(); i += 3) {
        const Array_<int> verts(&faceIndices[i], &faceIndices[i]+3);
        mesh.addFace(verts);
    }
}

struct VertKey {
    VertKey(const Vec3& v) : v(v) {}
    Vec3 v;
    bool operator<(const VertKey& other) const {
        const Real tol = SignificantReal;
        const Vec3 diff = v - other.v;
        if (diff[0] < -tol) return true;
        if (diff[0] >  tol) return false;
        if (diff[1] < -tol) return true;
        if (diff[1] >  tol) return false;
        if (diff[2] < -tol) return true;
        if (diff[2] >  tol) return false;
        return false; // they are numerically equal
    }
};
typedef std::map<VertKey,int> VertMap;

/* Search a list of vertices for one close enough to this one and
return its index if found, otherwise add to the end. */
static int getVertex(const Vec3& v, VertMap& vmap, Array_<Vec3>& verts) {
    VertMap::const_iterator p = vmap.find(VertKey(v));
    if (p != vmap.end()) return p->second;
    const int ix = (int)verts.size();
    verts.push_back(v);
    vmap.insert(std::make_pair(VertKey(v),ix));
    return ix;
}

/* Each face comes in as below, with vertices 0,1,2 on the surface
of a sphere or radius r centered at the origin. We bisect the edges to get
points a',b',c', then move out from the center to make points a,b,c
on the sphere.
         1
        /\        
       /  \
    c /____\ b      Then construct new triangles
     /\    /\            [0,b,a]
    /  \  /  \           [a,b,c]
   /____\/____\          [c,2,a]
  2      a     0         [b,1,c]
*/
static void refineSphere(Real r, VertMap& vmap, 
                         Array_<Vec3>& verts, Array_<int>&  faces) {
    assert(faces.size() % 3 == 0);
    const int nVerts = faces.size(); // # face vertices on entry
    for (int i=0; i < nVerts; i+=3) {
        const int v0=faces[i], v1=faces[i+1], v2=faces[i+2];
        const Vec3 a = r*UnitVec3(verts[v0]+verts[v2]);
        const Vec3 b = r*UnitVec3(verts[v0]+verts[v1]);
        const Vec3 c = r*UnitVec3(verts[v1]+verts[v2]);
        const int va=getVertex(a,vmap,verts), 
                  vb=getVertex(b,vmap,verts), 
                  vc=getVertex(c,vmap,verts);
        // Replace the existing face with the 0ba triangle, then add the rest.
        // Refer to the above picture.
        faces[i+1] = vb; faces[i+2] = va;
        faces.push_back(va); faces.push_back(vb); faces.push_back(vc);//abc
        faces.push_back(vc); faces.push_back(v2); faces.push_back(va);//c2a
        faces.push_back(vb); faces.push_back(v1); faces.push_back(vc);//b1c
    }
}

// level  numfaces
//   0       8   <-- octahedron
//   1       32
//   2       128 <-- still lumpy
//   3       512 <-- very spherelike
//   n       2*4^(n+1)
static void makeSphere(Real radius, int level, PolygonalMesh& sphere) {
    Array_<Vec3> vertices;
    Array_<int> faceIndices;
    makeOctahedralMesh(Vec3(radius), vertices, faceIndices);

    VertMap vmap;
    for (unsigned i=0; i < vertices.size(); ++i)
        vmap[vertices[i]] = i;

    while (level > 0) {
        refineSphere(radius, vmap, vertices, faceIndices);
        --level;
    }

    for (unsigned i=0; i < vertices.size(); ++i)
        sphere.addVertex(vertices[i]);
    for (unsigned i=0; i < faceIndices.size(); i += 3) {
        const Array_<int> verts(&faceIndices[i], &faceIndices[i]+3);
        sphere.addFace(verts);
    }
}
