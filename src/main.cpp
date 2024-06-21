#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE); // On peut configurer l'équation qui mélange deux couleurs, comme pour faire différents blend mode dans Photoshop. Cette équation-ci donne le blending "normal" entre pixels transparents.

    auto render_target = gl::RenderTarget{gl::RenderTarget_Descriptor{
        .width          = gl::framebuffer_width_in_pixels(),
        .height         = gl::framebuffer_height_in_pixels(),
        .color_textures = {
            gl::ColorAttachment_Descriptor{
                .format  = gl::InternalFormat_Color::RGBA32F,
                .options = {
                    .minification_filter  = gl::Filter::NearestNeighbour, // On va toujours afficher la texture à la taille de l'écran,
                    .magnification_filter = gl::Filter::NearestNeighbour, // donc les filtres n'auront pas d'effet. Tant qu'à faire on choisit le moins coûteux.
                    .wrap_x               = gl::Wrap::ClampToEdge,
                    .wrap_y               = gl::Wrap::ClampToEdge,
                },
            },
        },
        .depth_stencil_texture = gl::DepthStencilAttachment_Descriptor{
            .format  = gl::InternalFormat_DepthStencil::Depth32F,
            .options = {
                .minification_filter  = gl::Filter::NearestNeighbour,
                .magnification_filter = gl::Filter::NearestNeighbour,
                .wrap_x               = gl::Wrap::ClampToEdge,
                .wrap_y               = gl::Wrap::ClampToEdge,
            },
        },
    }};

    auto camera = gl::Camera{};
    gl::set_events_callbacks({
        {.on_framebuffer_resized = [&](gl::FramebufferResizedEvent const& e) {
            render_target.resize(e.width_in_pixels, e.height_in_pixels);
        }},
    });

    auto const shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
    }};

    auto const background_shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/background_vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/background_fragment.glsl"},
    }};

    auto const RenderTag_shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/RenderTarg_vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/RenderTarg_fragment.glsl"},
    }};

    auto const rectangle_mesh  = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0 /*Index de l'attribut dans le shader, on en reparle juste après*/}},
            .data   = {
                -0.5f, -0.5f, // Position2D du 1er sommet
                +0.5f, -0.5f, // Position2D du 2ème sommet
                +0.5f, +0.5f, // Position2D du 3ème sommet
                -0.5f, +0.5f  // Position2D du 4ème sommet
            },
        }},
        .index_buffer = {
            0, 1, 2, // Indices du premier triangle : on utilise le 1er, 2ème et 3ème sommet
            0, 2, 3  // Indices du deuxième triangle : on utilise le 1er, 3ème et 4ème sommet
        },
    }}; 

    auto const background_quad = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0}},
            .data   = {
                -1.0f, -1.0f,
                +1.0f, -1.0f,
                +1.0f, +1.0f,
                -1.0f, +1.0f
            },
        }},
        .index_buffer = {
            0, 1, 2,
            0, 2, 3
        },
    }};

    auto const RenderTag_quad = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {
                gl::VertexAttribute::Position2D{0}, // Index 0 pour la position
                gl::VertexAttribute::UV{1}         // Index 1 pour les UVs
            },
            .data = {
                // Positions   // UVs
                -1.0f, -1.0f,  0.0f, 0.0f,
                +1.0f, -1.0f,  1.0f, 0.0f,
                +1.0f, +1.0f,  1.0f, 1.0f,
                -1.0f, +1.0f,  0.0f, 1.0f
            },
        }},
        .index_buffer = {
            0, 1, 2,   // Premier triangle
            0, 2, 3    // Deuxième triangle
        },
    }};

    while (gl::window_is_open())
    {
        render_target.render([&]() {
            glClear(GL_DEPTH_BUFFER_BIT);
        
            shader.bind(); // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw(). On en reparle dans la section d'après.
            shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
            shader.set_uniform("time", gl::time_in_seconds());
            rectangle_mesh.draw(); // C'est ce qu'on appelle un "draw call" : on envoie l'instruction à la carte graphique de dessiner notre mesh.

            background_shader.bind();
            background_quad.draw();
        });

        glClear(GL_DEPTH_BUFFER_BIT);

        RenderTag_shader.bind();
        RenderTag_shader.set_uniform("screen_texture", render_target.color_texture(0));
        RenderTag_quad.draw();
    }
}