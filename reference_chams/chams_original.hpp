
// ================================================================================
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Telegram Bot: t.me/imguisource_bot
// ImGui Source: t.me/ImGui_Source_FIXNEY
// shit src 🧨: t.me/il2cpp_src
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ================================================================================

#pragma once

#include <il2cpp-api.h>
#include <il2cpp-class.h>
#include <il2cpp-object-include.h>
#include <il2cpp-tabledefs.h>
#include <string>
#include <vector>
#include <map>
#include <dirent.h>
#include <dlfcn.h>
#include <android/log.h>
#include "dobby.h" 
#include "offset.hpp" // Твой файл с оффсетами

#define LOG_TAG "ZeninChams"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using namespace Il2Cpp;

namespace Chams {
    struct Vector3 { float x, y, z; };
    struct Color { float r, g, b, a; };

    class AutoChams {
    private:
        void* chamsMaterial = nullptr;
        void* chamsShader = nullptr;
        void* originalRender = nullptr;
        bool isInitialized = false;

        struct PlayerState {
            void* visualObject = nullptr;
        };
        std::map<void*, PlayerState> playerStates;

        // Встроенный поиск файлов в папке
        std::string FindBundleInFolder(const std::string& folderPath) {
            DIR* dir = opendir(folderPath.c_str());
            if (!dir) return "";
            std::string foundPath = "";
            struct dirent* ent;
            while ((ent = readdir(dir)) != nullptr) {
                std::string fileName = ent->d_name;
                if (fileName == "." || fileName == ".." || fileName.find(".bundle") == std::string::npos) continue;
                foundPath = folderPath + "/" + fileName;
                break; 
            }
            closedir(dir);
            return foundPath;
        }

        // Внутренний хук рендера, который сам находит игроков
        static void Hook_CameraRender(void* __this, void* method) {
            AutoChams* instance = GetInstance();
            
            if (instance && instance->isInitialized) {
                // 1. Сами получаем базу libil2cpp.so
                uintptr_t baseAddress = (uintptr_t)dlopen("libil2cpp.so", RTLD_NOW);
                if (baseAddress) {
                    // 2. Сами читаем PlayerManager по твоему оффсету
                    uintptr_t managerAddr = baseAddress + offsets::base::player_manager();
                    void* playerManager = *(void**)managerAddr;

                    if (playerManager) {
                        instance->ProcessPlayers(playerManager);
                    }
                }
            }
            
            // Вызываем оригинал
            typedef void(*RenderFunc)(void*, void*);
            ((RenderFunc)instance->originalRender)(__this, method);
        }

        void ProcessPlayers(void* playerManager) {
            if (!chamsMaterial) return;

            void* localPlayer = *(void**)((uintptr_t)playerManager + offsets::manager::local());
            void* playerList = *(void**)((uintptr_t)playerManager + offsets::manager::list());
            if (!playerList) return;

            int listSize = *(int*)((uintptr_t)playerList + offsets::manager::list_size());
            void* buffer = *(void**)((uintptr_t)playerList + offsets::list::buffer());

            for (int i = 0; i < listSize; i++) {
                void* player = *(void**)((uintptr_t)buffer + (i * offsets::list::stride()) + offsets::list::entry());
                if (!player || player == localPlayer) continue;

                void* characterView = *(void**)((uintptr_t)player + offsets::player::character_view());
                if (!characterView) continue;

                void* bipedMap = *(void**)((uintptr_t)characterView + offsets::skeleton::biped_map());
                if (!bipedMap) continue;

                void* headBone = *(void**)((uintptr_t)bipedMap + offsets::skeleton::head());
                if (!headBone) continue;

                void* boneTransform = *(void**)((uintptr_t)headBone + offsets::skeleton::transform_object());
                if (!boneTransform) continue;

                ApplyChams(player, boneTransform);
            }
        }

        void ApplyChams(void* player, void* boneTransform) {
            auto it = playerStates.find(player);
            if (it == playerStates.end()) {
                // Создаем объект
                void* obj = GameObject_Internal_CreateGameObject(il2cpp_string_new("ChamsObj"), chamsShader);
                void* renderer = GameObject_AddComponent(obj, (Il2CppReflectionType*)Class_GetType(il2cpp_get_corlib()->image, "UnityEngine", "MeshRenderer"));
                Renderer_SetMaterial(renderer, chamsMaterial);
                
                PlayerState state;
                state.visualObject = obj;
                playerStates[player] = state;
                it = playerStates.find(player);
            }

            // Обновляем позицию (в твоем offset.hpp transform.position это 0x44)
            void* objTransform = GameObject_GetTransform(it->second.visualObject);
            Vector3 pos = *(Vector3*)((uintptr_t)boneTransform + offsets::transform::position());
            Transform_SetPosition(objTransform, pos);
        }

        static AutoChams*& GetInstance() {
            static AutoChams* instance = nullptr;
            return instance;
        }

        AutoChams() {
            GetInstance() = this;
        }

    public:
        // Единственный метод, который тебе нужно вызвать
        void Init(const std::string& bundleDir) {
            std::string bundlePath = FindBundleInFolder(bundleDir);
            if (bundlePath.empty()) {
                LOGI("Bundle not found in %s", bundleDir.c_str());
                return;
            }

            void* myBundle = AssetBundle_LoadFromFile(il2cpp_string_new(bundlePath.c_str()));
            if (!myBundle) {
                LOGI("Failed to load bundle");
                return;
            }

            chamsShader = AssetBundle_LoadAsset(myBundle, il2cpp_string_new("MyCustom/Chams"), (Il2CppReflectionType*)Class_GetType(il2cpp_get_corlib()->image, "UnityEngine", "Shader"));
            if (!chamsShader) {
                LOGI("Shader 'MyCustom/Chams' not found in bundle");
                return;
            }

            chamsMaterial = Material_New(chamsShader);
            Material_SetInt(chamsMaterial, il2cpp_string_new("_ZTest"), 8); // Always
            Material_SetInt(chamsMaterial, il2cpp_string_new("_ZWrite"), 0);
            Material_SetInt(chamsMaterial, il2cpp_string_new("_Cull"), 0);
            
            Color col = {0.0f, 1.0f, 0.0f, 1.0f}; // Зеленый
            Material_SetColor(chamsMaterial, il2cpp_string_new("_Color"), *(void**)&col);

            // Автоматический поиск и установка хука
            void* cameraClass = il2cpp_class_from_name(il2cpp_get_corlib()->image, "UnityEngine", "Camera");
            if (cameraClass) {
                void* renderMethod = il2cpp_class_get_method_from_name(cameraClass, "Render", 0);
                if (renderMethod) {
                    void* renderAddr = *(void**)renderMethod;
                    if (renderAddr) {
                        DobbyHook(renderAddr, (void*)Hook_CameraRender, &originalRender);
                        isInitialized = true;
                        LOGI("Chams initialized and hooked successfully!");
                    }
                }
            }
        }
        
        // Метод для изменения цвета из меню
        void SetColor(Color color) {
            if (chamsMaterial) {
                Material_SetColor(chamsMaterial, il2cpp_string_new("_Color"), *(void**)&color);
            }
        }
    };
}