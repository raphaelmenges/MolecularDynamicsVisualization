#ifndef PERFECT_SURFACE_DETECTION_H
#define PERFECT_SURFACE_DETECTION_H

#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// ### Create structure to find neighbors fast ###

// Do it later, could improve performance
// - determine size of volume and grid cell count
// - do something similar to per pixel linked list to save all atoms per cell
// - since only used in built up of cutting face list...really necessary?

// ### Calculate Solvent Accesible Surface ###

// Everything in one kernel? -> Yes :D Yolo
// - build up cutting face list (cut molecule with neighbors)
//  - input: atoms
//  - output: per atom: cutting face list
//  - algorithm: test atom's extented sphere against neighbors and save cutting planes
// - build up / use intersection line list (consisting of nodes storing up to two endpoints)
//  - (is in paper partley started in cutting face list buildup to stop if already internal)
//  - input: cutting face list per atom
//  - output: intersection line list or just boolean?
//  - algorithm:
//    - determine end points of intersection line of ALL pairs of cutting faces
//    - if end point not cut away by ANY other cutting face, then keep it in list
// - build array of indices of surface atoms
//   - if intersection line list is empty, add index of atom to some global memory

// TODO
// - Delete OpenGL stuff after usage
// - Work group size?
// - Binding points ok? not sure wheather atomic counter and image use the same

// Forward declaration instead of including (saved compile time)
class Protein;

// Class
class PerfectSurfaceDetection
{
public:

    // Constructor
    PerfectSurfaceDetection();

    // Render
    void renderLoop();

    // Keyboard callback for GLFW
    void keyCallback(int key, int scancode, int action, int mods);

private:

    // Atomic counter functions
    GLuint readAtomicCounter(GLuint atomicCounter) const;
    void resetAtomicCounter(GLuint atomicCounter) const;

    // Members
    GLFWwindow* mpWindow;
    std::unique_ptr<Protein> mupProtein;
    GLuint mSurfaceAtomTexture; // list of indices encoded in uint32
    GLint mSurfaceAtomCount;
};

#endif // PERFECT_SURFACE_DETECTION_H
