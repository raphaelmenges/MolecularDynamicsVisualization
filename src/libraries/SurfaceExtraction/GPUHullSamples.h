//============================================================================
// Distributed under the MIT License. Author: Raphael Menges
//============================================================================

// Samples of atoms' hull.

#ifndef GPU_HULL_SAMPLES_H
#define GPU_HULL_SAMPLES_H

#include "ShaderTools/ShaderProgram.h"
#include "SurfaceExtraction/GPUBuffer.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <functional>
#include <set>

// Forward declaration
class GPUProtein;
class GPUSurface;
class GPUTextureBuffer;

class GPUHullSamples
{
public:

    // Constructor
    GPUHullSamples();

    // Destructor
    virtual ~GPUHullSamples();

    // Computation. End frame determined by count of surfaces
    void compute(
        GPUProtein const * pGPUProtein,
        std::vector<std::unique_ptr<GPUSurface> > const * pGPUSurfaces,
        int startFrame,
        float probeRadius,
        int sampleCountPerAtom,
        unsigned int sampleSeed,
        std::function<void(float)> progressCallback = NULL);

    // Draw the computed samples
    void drawSamples(
        int frame, // absolute frame
        float pointSize,
        glm::vec3 internalSampleColor,
        glm::vec3 surfaceSampleColor,
        const glm::mat4& rViewMatrix,
        const glm::mat4& rProjectionMatrix,
        float clippingPlane) const;

    // Get count of samples in one frame (will stay same for all frames)
    int getProteinSampleCount() const { return mAtomCount * mSampleCount; }

    // Get count of all surface samples over all processed frames
    std::vector<GLuint> getSurfaceSampleCount() const { return mSurfaceSampleCount; }

    // Get count of surface samples of a certain atom group
    int getSurfaceSampleCount(int frame, std::set<GLuint> atomIndices) const;

    // Get count of surface samples of one atom
    int getSurfaceSampleCount(int frame, GLuint atomIndex) const;

    // Get count of surface samples in one specific frame
    int getSurfaceSampleCount(int frame) const;

    // Get count of samples for a given count of atoms
    int getSampleCount(int atomCount = 1) const;

private:

    // Remember start frame of computation
    int mStartFrame;

    // Count of atoms
    int mAtomCount;

    // Count of frames (defined by count of given calculated surfaces)
    int mLocalFrameCount;

    // Count of samples
    int mSampleCount;

    // Count of unsigned integers necessary for each sample
    int mIntegerCountPerSample;

    // Shader to compute classification
    std::unique_ptr<ShaderProgram> mupComputeProgram;

    // Vector with relative positions of samples
    // Is relative to atoms' centers
    std::vector<glm::vec3> mSamplesRelativePosition;

    // SSBO with relative positions of samples
    GPUBuffer<glm::vec3> mSamplesRelativePositionBuffer;

    // Texture buffer with information whether sample is on surface or not
    // Saved in single bits of unsigned integers in vector
    std::unique_ptr<GPUTextureBuffer> mupClassification;

    // Drawing shader
    std::unique_ptr<ShaderProgram> mupShaderProgram;

    // Vertex array object for drawing
    GLuint mVAO;

    // Vector for count of surface samples
    std::vector<GLuint> mSurfaceSampleCount;

    // Copy of classification results
    std::vector<GLuint> mClassification;

    // Pointer to GPUProtein used for rendering
    GPUProtein const * mpGPUProtein;
};

#endif // GPU_HULL_SAMPLES_H
