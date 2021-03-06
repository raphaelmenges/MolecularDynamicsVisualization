//============================================================================
// Distributed under the MIT License. Author: Raphael Menges
//============================================================================

#include "OrbitCamera.h"
#include <glm/gtc/matrix_transform.hpp>

const GLfloat CAMERA_BETA_BIAS = 0.0001f;

OrbitCamera::OrbitCamera(
        glm::vec3 center,
        GLfloat alpha,
        GLfloat beta,
        GLfloat radius,
        GLfloat minRadius,
        GLfloat maxRadius,
        GLfloat fov,
        GLfloat orthoScale)
{
    mCenter = center;
    mAlpha = alpha;
    mBeta = beta;
    mRadius = radius;
    mMinRadius = minRadius;
    mMaxRadius = maxRadius;
    mFov = fov;
    mOrthoScale = orthoScale;
    mPosition = glm::vec3(0, 0, 0);
    clampValues();
}

OrbitCamera::~OrbitCamera()
{
    // Nothing to do
}

void OrbitCamera::update(GLint viewportWidth, GLint viewportHeight, GLboolean perspective)
{
    // Save information to member
    mViewportWidth = viewportWidth;
    mViewportHeight = viewportHeight;

    // Calculate view and projection matrix
    if(perspective)
    {
        // Perspective
        mPosition.x = mRadius * glm::sin(glm::radians(mBeta)) * glm::cos(glm::radians(mAlpha));
        mPosition.y = mRadius * glm::cos(glm::radians(mBeta));
        mPosition.z = mRadius * glm::sin(glm::radians(mBeta)) * glm::sin(glm::radians(mAlpha));
        mPosition += mCenter;

        // View matrix
        mViewMatrix= glm::lookAt(mPosition, mCenter, glm::vec3(0.0f, 1.0f, 0.0f));

        // Projection matrix
        mProjectionMatrix = glm::perspective(
            glm::radians(mFov),
            (GLfloat)mViewportWidth / (GLfloat)mViewportHeight,
            0.1f,
            mMaxRadius * 2.f);
    }
    else
    {
        // Orthographic (max radius is used to calculate camera point)
        mPosition.x = mMaxRadius * glm::sin(glm::radians(mBeta)) * glm::cos(glm::radians(mAlpha));
        mPosition.y = mMaxRadius * glm::cos(glm::radians(mBeta));
        mPosition.z = mMaxRadius * glm::sin(glm::radians(mBeta)) * glm::sin(glm::radians(mAlpha));
        mPosition += mCenter;

        // View matrix
        mViewMatrix= glm::lookAt(mPosition, mCenter, glm::vec3(0.0f, 1.0f, 0.0f));

        // Projection matrix
        GLfloat halfWidth = ((GLfloat) mViewportWidth) * mOrthoScale * 0.5f;
        GLfloat halfHeight = ((GLfloat) mViewportHeight) * mOrthoScale * 0.5f;
        mOrthoZoom = glm::max(0.0000001f, (mRadius - mMinRadius) / (mMaxRadius - mMinRadius));
        mProjectionMatrix = glm::ortho(
            mOrthoZoom * -halfWidth,
            mOrthoZoom * halfWidth,
            mOrthoZoom * -halfHeight,
            mOrthoZoom * halfHeight,
            0.1f,
            mMaxRadius * 2.f);
    }
}

void OrbitCamera::reset(glm::vec3 center, GLfloat alpha, GLfloat beta, GLfloat radius)
{
    mCenter = center;
    mAlpha = alpha;
    mBeta = beta;
    mRadius = radius;
}

void OrbitCamera::setCenter(glm::vec3 center)
{
    mCenter = center;
}

void OrbitCamera::setAlpha(GLfloat alpha)
{
    mAlpha = alpha;
    clampAlpha();
}

void OrbitCamera::setBeta(GLfloat beta)
{
    mBeta = beta;
    clampBeta();
}

void OrbitCamera::setRadius(GLfloat radius)
{
    mRadius = radius;
    clampRadius();
}

glm::mat4 OrbitCamera::getViewMatrix() const
{
    return mViewMatrix;
}

glm::mat4 OrbitCamera::getProjectionMatrix() const
{
    return mProjectionMatrix;
}

glm::vec3 OrbitCamera::getPosition() const
{
    return mPosition;
}

glm::vec3 OrbitCamera::getCenter() const
{
    return mCenter;
}

GLfloat OrbitCamera::getAlpha() const
{
    return mAlpha;
}

GLfloat OrbitCamera::getBeta() const
{
    return mBeta;
}

GLfloat OrbitCamera::getRadius() const
{
    return mRadius;
}

glm::vec3 OrbitCamera::getDirection() const
{
    return glm::normalize(mCenter - mPosition);
}

glm::vec3 OrbitCamera::getPositionAtPixel(int x, int y) const
{
    // Direction is needed
    glm::vec3 direction = getDirection();

    // Calculate normalized vectors which span the view
    glm::vec3 a = glm::cross(glm::vec3(0,1,0), direction); // use up vector for cross product
    glm::vec3 b = glm::cross(direction, a);
    a = -glm::normalize(a); // change of direction because camera <-> world
    b = -glm::normalize(b); // change of direction because screen <-> world

    // Calculate position on pixel
    GLfloat multiplierA = mOrthoZoom * mOrthoScale * (GLfloat)(x - (mViewportWidth / 2));
    GLfloat multiplierB = mOrthoZoom * mOrthoScale * (GLfloat)(y - (mViewportHeight / 2));
    glm::vec3 positionOnPixel = mPosition + (a *  multiplierA) + (b * multiplierB);

    // Return result
    return positionOnPixel;
}

void OrbitCamera::clampValues()
{
    // Horizontal rotation
    clampAlpha();

    // Vertical rotation
    clampBeta();

    // Zoom/Radius
    clampRadius();
}

void OrbitCamera::clampAlpha()
{
    mAlpha = fmodf(mAlpha, 2 * glm::degrees(glm::pi<GLfloat>()));
    if (mAlpha < 0)
    {
        mAlpha = 2 * glm::degrees(glm::pi<GLfloat>()) + mAlpha;
    }
}

void OrbitCamera::clampBeta()
{
    mBeta = glm::clamp(mBeta, CAMERA_BETA_BIAS, glm::degrees(glm::pi<GLfloat>()) - CAMERA_BETA_BIAS);
}

void OrbitCamera::clampRadius()
{
    mRadius = glm::clamp(mRadius, mMinRadius, mMaxRadius);
}
