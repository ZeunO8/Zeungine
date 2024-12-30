#include <anex/modules/gl/shaders/ShaderFactory.hpp>
#include <anex/IWindow.hpp>
#include <stdexcept>
#include <glm/fwd.hpp>
using namespace anex::modules::gl::shaders;
ShaderFactory::ShaderHooksMap ShaderFactory::hooks = {
  {
    Shader::ShaderType::Vertex, {
      {
        "layout", {
          {
            "Color", {
              {
                ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string{
                  return "layout(location = " + std::to_string(ShaderFactory::currentInLayoutIndex++) + ") in vec4 inColor;";
                }
              },
              {
                ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string{
                  return "layout(location = " + std::to_string(ShaderFactory::currentOutLayoutIndex++) + ") out vec4 outColor;";
                }
              }
            }
          },
          {
            "Position", {
              {
                ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string{
                  return "layout(location = " + std::to_string(ShaderFactory::currentInLayoutIndex++) + ") in vec3 inPosition;";
                }
              },
              {
                ++ShaderFactory::hooksCount,  [](auto &shader, const auto &constants)->std::string{
                  return "layout(location = " + std::to_string(ShaderFactory::currentOutLayoutIndex++) + ") out vec3 outPosition;";
                }
              }
            }
          },
          {
            "View",
            {
              {
                ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string
                {
                  auto bindingIndex = ShaderFactory::currentBindingIndex++;
                  shader.addUBO("View", sizeof(glm::mat4), bindingIndex);
                  return "layout(binding = " + std::to_string(bindingIndex) + ") uniform View {\n" +
                    " mat4 matrix;\n" +
                    "} view;";
                }
              }
            }
          },
          {
            "Projection",
            {
                {
                  ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string
                  {
                    auto bindingIndex = ShaderFactory::currentBindingIndex++;
                    shader.addUBO("Projection", sizeof(glm::mat4), bindingIndex);
                    return "layout(binding = " + std::to_string(bindingIndex) + ") uniform Projection {\n" +
                      " mat4 matrix;\n" +
                      "} projection;";
                  }
                }
            }
          },
          {
            "Model",
            {
                  {
                    ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string
                    {
                      auto bindingIndex = ShaderFactory::currentBindingIndex++;
                      shader.addUBO("Model", sizeof(glm::mat4), bindingIndex);
                      return "layout(binding = " + std::to_string(bindingIndex) + ") uniform Model {\n" +
                        " mat4 matrix;\n" +
                        "} model;";
                    }
                  }
            }
          }
        }
      },
      {
        "preInMain", {
          {
            "Color", {
              {
                ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string{
                  return "  outColor = inColor;";
                }
              }
            }
          },
          {
            "Position", {
              {
                ++ShaderFactory::hooksCount, [](auto &shader, const RuntimeConstants &constants)->std::string{
                  std::string string = "  gl_Position = ";
                  if (std::find(constants.begin(), constants.end(), "Projection") != constants.end())
                  {
                    string += "projection.matrix * ";
                  }
                  if (std::find(constants.begin(), constants.end(), "View") != constants.end())
                  {
                    string += "view.matrix * ";
                  }
                  if (std::find(constants.begin(), constants.end(), "Model") != constants.end())
                  {
                    string += "model.matrix * ";
                  }
                  string += "vec4(inPosition, 1);\n";
                  string += "outPosition = vec3(gl_Position);";
                  return string;
                }
              }
            }
          }
        }
      }
    }
  },
  {
    Shader::ShaderType::Fragment, {
      {
        "layout",
        {
          {
            "Color", {
              {
                ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string{
                  return "layout(location = " + std::to_string(ShaderFactory::currentInLayoutIndex++) + ") in vec4 inColor;";
                },
              },
              {
                ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string{
                  return "layout(location = " + std::to_string(ShaderFactory::currentOutLayoutIndex++) + ") out vec4 FragColor;";
                }
              }
            }
          },
          {
            "Position", {
              {
                ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string{
                  return "layout(location = " + std::to_string(ShaderFactory::currentInLayoutIndex++) + ") in vec3 inPosition;";
                }
              }
            }
          },
          {
            "CameraPosition",
            {
                    {
                      ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string
                      {
                        auto bindingIndex = ShaderFactory::currentBindingIndex++;
                        shader.addUBO("CameraPosition", sizeof(glm::vec3), bindingIndex);
                        return "layout(binding = " + std::to_string(bindingIndex) + ") uniform CameraPosition {\n" +
                          " vec3 value;\n" +
                          "} cameraPosition;";
                      }
                    }
            }
          },
          {
            "Fog", {
                {
                  ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string {
                    return "uniform float fogDensity;\nuniform vec4 fogColor;";
                  }
                }
            }
          }
        }
      },
      {
        "preMain", {
              {
                "Fog", {
                  {
                    ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string {
                      return "float calculateFogFactor(float distance, float density);";
                    }
                  }
                }
              }
        },
      },
      {
        "preInMain",
        {
          {
            "Color", {
              {
                ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string{
                  return "  FragColor = inColor;";
                }
              }
            }
          }
        }
      },
      {
        "postInMain", {
                {
                  "Fog", {
                    {
                      ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string{
                        return std::string("float distance = length(inPosition - cameraPosition.value);") +
                          "float fogFactor = calculateFogFactor(distance, fogDensity);" +
                          "FragColor = mix(fogColor, FragColor, fogFactor);";
                      }
                    }
                  }
                }
        }
      },
      {
        "postMain", {
              {
                "Fog", {
                  {
                    ++ShaderFactory::hooksCount, [](auto &shader, const auto &constants)->std::string{
                      return std::string("float calculateFogFactor(float distance, float density) {\n") +
                        " return exp(-density * distance);\n" +
                        "}";
                    }
                  }
                }
              }
        }
      }
    }
  }
};
uint32_t ShaderFactory::hooksCount = 0;
ShaderFactory::ShaderHookInfoMap ShaderFactory::shaderHookInfos;
ShaderFactory::ShaderTypeMap ShaderFactory::shaderTypes = {
  {Shader::ShaderType::Vertex, GL_VERTEX_SHADER},
  {Shader::ShaderType::Geometry, GL_GEOMETRY_SHADER},
  {Shader::ShaderType::Fragment, GL_FRAGMENT_SHADER},
  {Shader::ShaderType::TessellationControl, GL_TESS_CONTROL_SHADER},
  {Shader::ShaderType::TessellationEvaluation, GL_TESS_EVALUATION_SHADER},
  {Shader::ShaderType::Compute, GL_COMPUTE_SHADER}
};
ShaderFactory::ShaderNameMap ShaderFactory::shaderNames = {
  {Shader::ShaderType::Vertex, "VertexShader"},
  {Shader::ShaderType::Geometry, "GeometryShader"},
  {Shader::ShaderType::Fragment, "FragmentShader"},
  {Shader::ShaderType::TessellationControl, "TessellationControlShader"},
  {Shader::ShaderType::TessellationEvaluation, "TessellationEvaluationShader"},
  {Shader::ShaderType::Compute, "ComputeShader"}
};
uint32_t ShaderFactory::currentInLayoutIndex = 0;
uint32_t ShaderFactory::currentOutLayoutIndex = 0;
uint32_t ShaderFactory::currentBindingIndex = 0;
Shader::ShaderMap ShaderFactory::generateShaderMap(const RuntimeConstants &constants, Shader &shader)
{
  Shader::ShaderMap shaderMap;
  shaderMap[Shader::ShaderType::Vertex] = generateShader(Shader::ShaderType::Vertex, constants, shader);
  shaderMap[Shader::ShaderType::Fragment] = generateShader(Shader::ShaderType::Fragment, constants, shader);
  return shaderMap;
};
Shader::ShaderPair ShaderFactory::generateShader(const Shader::ShaderType &shaderType, const RuntimeConstants &constants, Shader &shader)
{
  Shader::ShaderPair shaderPair;
  auto &shaderString = shaderPair.first;
  auto &shaderHooks = hooks[shaderType];
  shaderString += "#version 430 core\n";
  currentInLayoutIndex = 0;
  currentOutLayoutIndex = 0;
  currentBindingIndex = 0;
  appendHooks(shaderString, shaderHooks["layout"], constants, shader);
  appendHooks(shaderString, shaderHooks["preMain"], constants, shader);
  shaderString += "void main()\n{\n";
  appendHooks(shaderString, shaderHooks["preInMain"], constants, shader);
  appendHooks(shaderString, shaderHooks["postInMain"], constants, shader);
  shaderString += "}\n";
  appendHooks(shaderString, shaderHooks["postMain"], constants, shader);
  if (!compileShader(shaderType, shaderPair))
  {
    throw std::runtime_error("Failed to compile fragment shader");
  }
  return shaderPair;
};
void ShaderFactory::appendHooks(std::string &shaderString, RuntimeHooksMap &runtimeHooks, const RuntimeConstants &constants, Shader &shader)
{
  for (const std::string &constant : constants)
  {
    const auto &constantHooks = runtimeHooks[constant];
    for (auto &hook : constantHooks)
    {
      shaderString += hook.second(shader, constants) + "\n";
    }
  }
}
bool ShaderFactory::compileShader(const Shader::ShaderType &shaderType, Shader::ShaderPair &shaderPair)
{
  auto &shaderString = shaderPair.first;
  auto &shaderInt = shaderPair.second;
  shaderInt = glCreateShader(shaderTypes[shaderType]);
  const GLchar* source = shaderString.c_str();
  GLint lengths[] = { (GLint)shaderString.size() };
  glShaderSource(shaderInt, 1, &(source), lengths);
  glCompileShader(shaderInt);
  return checkCompileErrors(shaderInt, true, shaderNames[shaderType].c_str());
};
bool ShaderFactory::compileProgram(const Shader::ShaderMap &shaderMap, GLuint &program)
{
  program = glCreateProgram();
  for (const auto& shaderMapPair : shaderMap)
  {
    glAttachShader(program, shaderMapPair.second.second);
  }
  glLinkProgram(program);
  for (const auto& shaderMapPair : shaderMap)
  {
    glDeleteShader(shaderMapPair.second.second);
  }
  return checkCompileErrors(program, false, "Program");
};
const bool ShaderFactory::checkCompileErrors(const GLuint& id, const bool& isShader, const char* shaderType)
{
  GLint success = 0;
  if (isShader)
  {
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      GLint infoLogLength;
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
      GLchar* infoLog = new GLchar[infoLogLength + 1];
      glGetShaderInfoLog(id, infoLogLength, 0, infoLog);
      printf("SHADER_COMPILATION_ERROR(%s):\n%s\n", shaderType, infoLog);
      delete[] infoLog;
      return false;
    }
  }
  else
  {
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
      GLint infoLogLength;
      glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
      GLchar* infoLog = new GLchar[infoLogLength + 1];
      glGetProgramInfoLog(id, infoLogLength, 0, infoLog);
      printf("PROGRAM_LINKING_ERROR(%s):\n%s\n", shaderType, infoLog);
      delete[] infoLog;
      return false;
    }
  }
  return true;
};
void ShaderFactory::deleteProgram(Shader &shader)
{
  glDeleteProgram(shader.program);
};
uint32_t ShaderFactory::addHook(const Shader::ShaderType &shaderType, const std::string &hookName, const std::string &runtimeConstant, const Shader::ShaderHook &hook)
{
  auto id = ++hooksCount;
  hooks[shaderType][hookName][runtimeConstant].emplace(id, hook);
  shaderHookInfos[id] = {shaderType, hookName, runtimeConstant};
  return id;
};
void ShaderFactory::deleteHook(const uint32_t& id)
{
  auto infoIter = shaderHookInfos.find(id);
  if (infoIter == shaderHookInfos.end())
    return;
  auto hooksIter = hooks.find(std::get<0>(infoIter->second));
  if (hooksIter == hooks.end())
    return;
  auto hookIter = hooksIter->second.find(std::get<1>(infoIter->second));
  if (hookIter == hooksIter->second.end())
    return;
  auto runtimeHookIter = hookIter->second.find(std::get<2>(infoIter->second));
  if (runtimeHookIter == hookIter->second.end())
    return;
  runtimeHookIter->second.erase(id);
};