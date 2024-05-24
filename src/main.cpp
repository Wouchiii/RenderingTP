#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut

    glEnable(GL_DEPTH_TEST);

    auto camera = gl::Camera{};
    gl::set_events_callbacks({camera.events_callbacks()});

    auto const shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
    }};

    auto const cube_mesh  = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position3D{0 /*Index de l'attribut dans le shader, on en reparle juste après*/}},
            .data   = {
                -0.5f, +0.5f, +0.5f,    // Sommet 0
                +0.5f, +0.5f, +0.5f,    // Sommet 1
                +0.5f, -0.5f, +0.5f,    // Sommet 2
                -0.5f, -0.5f, +0.5f,    // Sommet 3
                -0.5f, +0.5f, -0.5f,    // Sommet 4
                +0.5f, +0.5f, -0.5f,    // Sommet 5
                +0.5f, -0.5f, -0.5f,    // Sommet 6
                -0.5f, -0.5f, -0.5f     // Sommet 7 
            },
        }},
        .index_buffer = {          
            0, 3, 2,          
            0, 1, 2,          
            0, 4, 7,          
            0, 3, 7,
            4, 7, 6,
            4, 5, 6,
            5, 6, 2,
            5, 1, 2,
            2, 6, 3,
            3, 7, 6,
            0, 1, 5,
            0, 4, 5
        },
    }};  

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 const view_matrix = camera.view_matrix();
        glm::mat4 const projection_matrix = glm::infinitePerspective(1.f /*field of view in radians*/, gl::framebuffer_aspect_ratio() /*aspect ratio*/, 0.001f /*near plane*/);
        
        shader.bind(); // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw(). On en reparle dans la section d'après.
        shader.set_uniform("view_projection_matrix", projection_matrix * view_matrix);

        cube_mesh.draw(); // C'est ce qu'on appelle un "draw call" : on envoie l'instruction à la carte graphique de dessiner notre mesh.
    }
}