#include "index.hpp"

// https://mynameismjp.wordpress.com/2015/02/18/shadow-sample-update/

// [ ] Stencil Reflections + Shadows
// [ ] Shadow Volumes (face / edge)
// [ ] Simple Shadow Mapping (SSM)
// [ ] Variance Shadow Mapping (VSM)
// [ ] Exponential Shadow Mapping (ESM)
// [ ] Cascaded Shadow Mapping (CSM)
// [ ] Percentage Closer Filtering (PCF) + poisson disk sampling
// [ ] Moment Shadow Mapping [MSM]

std::shared_ptr<GlShader> make_watched_shader(ShaderMonitor & mon, const std::string vertexPath, const std::string fragPath)
{
    std::shared_ptr<GlShader> shader = std::make_shared<GlShader>(read_file_text(vertexPath), read_file_text(fragPath));
    mon.add_shader(shader, vertexPath, fragPath);
    return shader;
}

struct ExperimentalApp : public GLFWApp
{
    uint64_t frameCount = 0;

    GlCamera camera;
    PreethamProceduralSky skydome;
    FlyCameraController cameraController;
    ShaderMonitor shaderMonitor;
    std::unique_ptr<gui::ImGuiManager> igm;
    
    ExperimentalApp() : GLFWApp(1280, 720, "Shadow Mapping App")
    {
        glfwSwapInterval(0);
        
        igm.reset(new gui::ImGuiManager(window));
        gui::make_dark_theme();
        
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        cameraController.set_camera(&camera);
        camera.look_at({0, 2.5, -2.5}, {0, 2.0, 0});
        
        gl_check_error(__FILE__, __LINE__);
    }
    
    void on_window_resize(int2 size) override
    {

    }
    
    void on_input(const InputEvent & e) override
    {
        if (igm) igm->update_input(e);
        cameraController.handle_input(e);
    }
    
    void on_update(const UpdateEvent & e) override
    {
        cameraController.update(e.timestep_ms);
        shaderMonitor.handle_recompile();
    }
    
    void on_draw() override
    {
        auto lightDir = skydome.get_light_direction();
        auto sunDir = skydome.get_sun_direction();
        auto sunPosition = skydome.get_sun_position();
        
        glfwMakeContextCurrent(window);
        
        if (igm) igm->begin_frame();
        
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
     
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1f, 0.5f, 1.0f);

        const auto proj = camera.get_projection_matrix((float) width / (float) height);
        const float4x4 view = camera.get_view_matrix();
        const float4x4 viewProj = mul(proj, view);
        
        skydome.render(viewProj, camera.get_eye_point(), camera.farClip);

        gl_check_error(__FILE__, __LINE__);
        
        if (igm) igm->end_frame();
        
        glfwSwapBuffers(window);
    }
    
};