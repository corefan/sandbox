#include "index.hpp"

constexpr const char colorVertexShader[] = R"(#version 330
    layout(location = 0) in vec3 vertex;
    layout(location = 1) in vec3 vnorm;
    uniform mat4 u_modelMatrix;
    uniform mat4 u_modelMatrixIT;
    uniform mat4 u_viewProj;
    uniform vec3 u_color;
    out vec3 color;
    out vec3 normal;
    void main()
    {
        vec4 worldPos = u_modelMatrix * vec4(vertex, 1);
        gl_Position = u_viewProj * worldPos;
        color = u_color * 0.80;
        normal = normalize((u_modelMatrixIT * vec4(vnorm,0)).xyz);
    }
)";

constexpr const char colorFragmentShader[] = R"(#version 330
    in vec3 color;
    out vec4 f_color;
    in vec3 normal;
    void main()
    {
        f_color = (vec4(color.rgb, 1) * 0.75)+ (dot(normal, vec3(0, 1, 0)) * 0.33);
    }
)";

std::shared_ptr<GlShader> make_watched_shader(ShaderMonitor & mon, const std::string vertexPath, const std::string fragPath, const std::string geomPath = "")
{
    std::shared_ptr<GlShader> shader = std::make_shared<GlShader>(read_file_text(vertexPath), read_file_text(fragPath), read_file_text(geomPath));
    mon.add_shader(shader, vertexPath, fragPath);
    return shader;
}

struct ExperimentalApp : public GLFWApp
{
    uint64_t frameCount = 0;

    GlCamera camera;
    RenderableGrid grid;
    FlyCameraController cameraController;
    ShaderMonitor shaderMonitor;
    
    std::unique_ptr<GizmoEditor> gizmoEditor;
    std::vector<Renderable> proceduralModels;

    std::shared_ptr<GlShader> colorShader;
    std::shared_ptr<GlShader> pbrShader;
    
    TexturedMesh sponza;
    
    ExperimentalApp() : GLFWApp(1200, 800, "Manipulation App")
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        grid = RenderableGrid(1, 100, 100);
        cameraController.set_camera(&camera);
        
        gizmoEditor.reset(new GizmoEditor(camera));
        
        colorShader.reset(new GlShader(colorVertexShader, colorFragmentShader));
        
        pbrShader = make_watched_shader(shaderMonitor, "assets/shaders/untextured_pbr_vert.glsl", "assets/shaders/untextured_pbr_frag.glsl");
        
        proceduralModels.resize(6);
        for (int i = 0; i < proceduralModels.size(); ++i)
        {
            proceduralModels[i] = Renderable(make_cube());
            proceduralModels[i].pose.position = float3(5 * sin(i), +2, 5 * cos(i));
        }
    }
    
    void on_window_resize(int2 size) override
    {

    }
    
    void on_input(const InputEvent & event) override
    {
        gizmoEditor->handle_input(event, proceduralModels);
        cameraController.handle_input(event);
    }
    
    void on_update(const UpdateEvent & e) override
    {
        cameraController.update(e.timestep_ms);
        shaderMonitor.handle_recompile();
    }
    
    void on_draw() override
    {
        glfwMakeContextCurrent(window);
        
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
     
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        const auto proj = camera.get_projection_matrix((float) width / (float) height);
        const float4x4 view = camera.get_view_matrix();
        const float4x4 viewProj = mul(proj, view);
        
        // Models
        {
            colorShader->bind();
            
            colorShader->uniform("u_viewProj", viewProj);

            for (const auto & model : proceduralModels)
            {
                colorShader->uniform("u_modelMatrix", model.get_model());
                colorShader->uniform("u_modelMatrixIT", inv(transpose(model.get_model())));
                colorShader->uniform("u_color", {1, 1, 1});
                model.draw();
            }
            
            colorShader->unbind();
        }
        
        // Gizmo
        {
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1, -1);
            
            colorShader->bind();
            
            colorShader->uniform("u_viewProj", viewProj);
            
            if (gizmoEditor->get_selected_object())
            {
                Renderable * selectedObject = gizmoEditor->get_selected_object();
                
                for (auto axis : {float3(1, 0, 0), float3(0, 1, 0), float3(0, 0, 1)})
                {
                    auto p = selectedObject->pose * Pose(make_rotation_quat_between_vectors({1,0,0}, axis), {0,0,0});
                    colorShader->uniform("u_modelMatrix", p.matrix());
                    colorShader->uniform("u_modelMatrixIT", inv(transpose(p.matrix())));
                    colorShader->uniform("u_color", axis);
                    gizmoEditor->get_gizmo_mesh().draw();
                }
            }

            colorShader->unbind();
        }
        
        grid.render(proj, view, {0, -0.5, 0});

        gl_check_error(__FILE__, __LINE__);
        
        glfwSwapBuffers(window);
        
        frameCount++;
    }
    
};