#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut

    glEnable(GL_DEPTH_TEST);

    auto const texture = gl::Texture{
        gl::TextureSource::File{ // Peut être un fichier, ou directement un tableau de pixels
            .path           = "res/texture.png",
            .flip_y         = true, // Il n'y a pas de convention universelle sur la direction de l'axe Y. Les fichiers (.png, .jpeg) utilisent souvent une direction différente de celle attendue par OpenGL. Ce booléen flip_y est là pour inverser la texture si jamais elle n'apparaît pas dans le bon sens.
            .texture_format = gl::InternalFormat::RGBA8, // Format dans lequel la texture sera stockée. On pourrait par exemple utiliser RGBA16 si on voulait 16 bits par canal de couleur au lieu de 8. (Mais ça ne sert à rien dans notre cas car notre fichier ne contient que 8 bits par canal, donc on ne gagnerait pas de précision). On pourrait aussi stocker en RGB8 si on ne voulait pas de canal alpha. On utilise aussi parfois des textures avec un seul canal (R8) pour des usages spécifiques.
        },
        gl::TextureOptions{
            .minification_filter  = gl::Filter::Linear, // Comment on va moyenner les pixels quand on voit l'image de loin ?
            .magnification_filter = gl::Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
            .wrap_x               = gl::Wrap::Repeat,   // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y               = gl::Wrap::Repeat,   // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
        }
    };

    auto render_target = gl::RenderTarget{gl::RenderTarget_Descriptor{
        .width          = gl::framebuffer_width_in_pixels(),
        .height         = gl::framebuffer_height_in_pixels(),
        .color_textures = {
            gl::ColorAttachment_Descriptor{
                .format  = gl::InternalFormat_Color::RGBA8,
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
    gl::set_events_callbacks({camera.events_callbacks()});

    gl::set_events_callbacks({
        camera.events_callbacks(),
        {.on_framebuffer_resized = [&](gl::FramebufferResizedEvent const& e) {
            render_target.resize(e.width_in_pixels, e.height_in_pixels);
        }},
    });

    auto const shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
    }};

    auto const quad_shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/background_vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/background_fragment.glsl"},
    }};

    auto const cube_mesh  = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {
                gl::VertexAttribute::Position3D{0},
                gl::VertexAttribute::UV{1}
            },
            .data   = {
                // Position                 // UV
                -0.5f, -0.5f, -0.5f,        0.0f, 0.0f,
                +0.5f, -0.5f, -0.5f,        1.0f, 0.0f,
                +0.5f, +0.5f, -0.5f,        1.0f, 1.0f,
                -0.5f, +0.5f, -0.5f,        0.0f, 1.0f,
                -0.5f, -0.5f, +0.5f,        0.0f, 0.0f,
                +0.5f, -0.5f, +0.5f,        1.0f, 0.0f,
                +0.5f, +0.5f, +0.5f,        1.0f, 1.0f,
                -0.5f, +0.5f, +0.5f,        0.0f, 1.0f
            },
        }},
        .index_buffer = {     
            0, 1, 2, 0, 2, 3, // Front face
            1, 5, 6, 1, 6, 2, // Right face
            5, 4, 7, 5, 7, 6, // Back face
            4, 0, 3, 4, 3, 7, // Left face
            3, 2, 6, 3, 6, 7, // Top face
            4, 5, 1, 4, 1, 0  // Bottom face
        },
    }};  

    auto const screen_quad = gl::Mesh{{
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
            glClearColor(1.f, 0.f, 0.f, 1.f); // Dessine du rouge, non pas à l'écran, mais sur notre render target
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 const view_matrix = camera.view_matrix();
            glm::mat4 const projection_matrix = glm::infinitePerspective(1.f /*field of view in radians*/, gl::framebuffer_aspect_ratio() /*aspect ratio*/, 0.001f /*near plane*/);
        
            shader.bind(); // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw(). On en reparle dans la section d'après.
            shader.set_uniform("view_projection_matrix", projection_matrix * view_matrix);
            shader.set_uniform("my_texture", texture);

            cube_mesh.draw(); // C'est ce qu'on appelle un "draw call" : on envoie l'instruction à la carte graphique de dessiner notre mesh.
        });

        quad_shader.bind();
        quad_shader.set_uniform("screen_texture", );

        screen_quad.draw();
    }
}