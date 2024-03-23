#define BOOST_TEST_MODULE GLM Tests
#include <boost/test/included/unit_test.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

/**
 * Vector Addition
 * Elementwise Vector Multiplication
 * Transpose 2x2 3x3 4x4
 * Matrix Vector Multiplication
 * Matrix-Matrix Multiplication
 */

BOOST_AUTO_TEST_CASE(AddVectors)
{
    // Confirm vector addition
    glm::vec3 u(1.0f, 2.0f, 3.0f);
    glm::vec3 v(-1.0f, 1.0f, -2.0f);

    glm::vec3 result = u + v;
    glm::vec3 expected(0.0f, 3.0f, 1.0f);
    BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(HadamardProduct)
{
    glm::vec3 u(1.0, 2.0, 3.0);
    glm::vec3 v(2.0, 2.0, 2.0);

    BOOST_CHECK(u * v == glm::vec3(2.0, 4.0, 6.0));
}

BOOST_AUTO_TEST_CASE(Transpose2x2)
{
    glm::mat2 A 
    {{ 1, 2 },
     { 3, 4 }};

    glm::mat2 B
    {{ 1, 3 },
     { 2, 4 }};

    BOOST_CHECK(glm::transpose(A) == B);
    BOOST_CHECK(glm::transpose(B) == A);
}

BOOST_AUTO_TEST_CASE(Transpose3x3)
{
    glm::mat3 A
    {{ 1, 2, 3 },
     { 4, 5, 6 },
     { 7, 8, 9 }};
    glm::mat3 B
    {{ 1, 4, 7 },
     { 2, 5, 8 },
     { 3, 6, 9 }};
    BOOST_CHECK(glm::transpose(A) == B);
    BOOST_CHECK(glm::transpose(B) == A);
}

BOOST_AUTO_TEST_CASE(Transpose4x4)
{
    glm::mat4 A
    {{ 1, 2, 3, 4 },
     { 5, 6, 7, 8 },
     { 9, 10, 11, 12},
     { 13, 14, 15, 16 }};
    glm::mat4 B
    {{ 1, 5, 9, 13},
     { 2, 6, 10, 14},
     { 3, 7, 11, 15},
     { 4, 8, 12, 16}};
    BOOST_CHECK(glm::transpose(A) == B);
    BOOST_CHECK(glm::transpose(B) == A);
}

BOOST_AUTO_TEST_CASE(MatVecMul)
{
    glm::mat4 A
    {{1.0, 2.0, -1.0, -2.0},
     {3.0, 1.0, 2.0, -1.0},
     {5.0, 1.0, 7.0, 0.0},
     {1.0, 2.0, 4.0, 1.0}};

    glm::vec4 v(1.0, -1.0, 2.0, 4.0);
    glm::vec4 result = glm::transpose(A) * v;
    glm::vec4 expected(-11.0, 2.0, 18.0, 11.0);

    BOOST_CHECK(result == expected);
}

BOOST_AUTO_TEST_CASE(MatMatMul)
{
    glm::mat4 A
    {{  1, -2,  3, -1 },
     { -3,  2,  4,  5 },
     { -2, -1, -4,  3 },
     {  7, -1, -1,  2 }};

    glm::mat4 B
    {{  2,  3, -4,  5 },
     { -1, -2,  7, -2 },
     { -1, -1,  1,  3 },
     {  4,  3,  2, -8 }};

    glm::mat4 expected 
    {{ -3,  1, -17,  26 },
     {  8, -2,  40, -47 },
     { 13,  9,   3, -44 },
     { 24, 30, -32,  18 }};

    glm::mat4 result = glm::transpose(glm::transpose(A) * glm::transpose(B));

    BOOST_CHECK(expected == result);
}

