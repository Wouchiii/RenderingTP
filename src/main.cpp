#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"
#include <glm/ext/matrix_transform.hpp>


static std::vector<float> LoadObj(const char* filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    std::vector<float> vertices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            glm::vec4 pos(
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2],
                1.0f
            );
            pos = rotation * pos;

             glm::vec3 normal = glm::vec3(
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2]
            );
            normal = glm::mat3(rotation) * normal;

            vertices.push_back(pos.x);
            vertices.push_back(pos.y);
            vertices.push_back(pos.z);

            if (index.texcoord_index >= 0) {
                vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]); // u
                vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]); // v
            }

            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }
    return vertices;
}



int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut

    glEnable(GL_DEPTH_TEST);

    auto const texture = gl::Texture{
        gl::TextureSource::File{
            .path           = "res/Model3D/fourareen2K_albedo.jpg",
            .flip_y         = true,
            .texture_format = gl::InternalFormat::RGBA8,
        },
        gl::TextureOptions{
            .minification_filter  = gl::Filter::Linear,
            .magnification_filter = gl::Filter::Linear,
            .wrap_x               = gl::Wrap::Repeat,
            .wrap_y               = gl::Wrap::Repeat,
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
        if(e.width_in_pixels != 0 && e.height_in_pixels != 0) // OpenGL crash si on tente de faire une render target avec une taille de 0
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
                gl::VertexAttribute::UV{1},
                gl::VertexAttribute::Normal3D{2}
            },
            .data = LoadObj(gl::make_absolute_path("res/Model3D/fourareen.obj").string().c_str()),
        }},
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        quad_shader.bind();
        quad_shader.set_uniform("screen_texture", render_target.color_texture(0));

        screen_quad.draw();
    }
}