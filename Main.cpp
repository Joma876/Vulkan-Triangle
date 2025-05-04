#define GLFW_INCLUDE_VULKAN 
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <vector>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateDebugInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"); 
	if (func != nullptr) return func(instance, pCreateDebugInfo, pAllocator, pDebugMessenger);
	else return VK_ERROR_EXTENSION_NOT_PRESENT; 
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"); 
	if (func != nullptr) func(instance, debugMessenger, pAllocator); 
}
class HelloTriangleApp {

private: 

	//GLFW Variables

	GLFWwindow* window; 

	const uint32_t WIDTH = 800; 
	const uint32_t HEIGHT = 600; 

	//Vk Variables

	VkInstance instance; 
	VkDebugUtilsMessengerEXT debugMessenger; 
		//Vk Validation Layers 
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" }; 
		#ifdef  NDEBUG
			const bool enabelValidationLayer = true; 
		#else
			const bool enableValidationLayer = true; 
		#endif //  NDEBUG


private: 
	//GLFW functions
	void initWindow(); 

	//Vk functions
	void initVulkan();

	//Struct Creation functions
	void createInstance(); 
	void createInfo(VkApplicationInfo& appInfo); 
	void setupDebugMessenger(); 

	//Check functions
	std::vector<const char*> getRequierdExtensions(); 
	bool validExtensionsSupport(std::vector<const char*> RequiredExtensions, std::vector<VkExtensionProperties>& extensions);
	bool validValidationLayerSupport(); 

	//Debug Message functions
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagBitsEXT MessageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData); 

	void mainloop(); 
	void cleanup(); 

public: 

	void run() {
		initWindow(); 
		initVulkan(); 

		mainloop(); 
		cleanup(); 
	}
};

//Initaliazation Functions

void HelloTriangleApp::initWindow() {
	glfwInit(); 

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); 
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); 

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Triangle", nullptr, nullptr); 
}

void HelloTriangleApp::initVulkan() {
	createInstance(); 
	setupDebugMessenger();
}

//Struct Creation Functions

void HelloTriangleApp::createInstance() {

	if (enableValidationLayer && !validValidationLayerSupport()) {
		throw std::runtime_error("validation Layer Suppoert requested, but not available!");
	}

	VkApplicationInfo appInfo{}; 

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO; 
	appInfo.pApplicationName = "Vulkan Triangle"; 
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0); 
	appInfo.pEngineName = "No Engine"; 
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0); 
	appInfo.apiVersion = VK_API_VERSION_1_0; 

	createInfo(appInfo); 
}

void HelloTriangleApp::createInfo(VkApplicationInfo& appInfo) {
	VkInstanceCreateInfo createInfo{}; 
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO; 
	createInfo.pApplicationInfo = &appInfo; 

	std::vector<const char*> RequiredExtensions = getRequierdExtensions(); 

	createInfo.enabledExtensionCount = static_cast<uint32_t>(RequiredExtensions.size()); 
	createInfo.ppEnabledExtensionNames = RequiredExtensions.data(); 
	
	uint32_t extensionCount = 0; 
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount); 
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()); 

		std::cout << "Required extensions\n";
		for (const auto& Extension : RequiredExtensions) {
			std::cout << "\t" << Extension << "\n";
		}
		std::cout << std::endl;

		std::cout << "available extensions: \n"; 

		for (const auto& extension : extensions) {
			std::cout << "\t" << extension.extensionName << "\n"; 
		}

	if (enableValidationLayer) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else createInfo.enabledLayerCount = 0; 

	if (!validExtensionsSupport(RequiredExtensions, extensions)) throw std::runtime_error("Invalid Extensions"); 

	if (vkCreateInstance(&createInfo, nullptr, &instance)) throw std::runtime_error("failed to create Instance");
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); 
}

void HelloTriangleApp::setupDebugMessenger() {
	if (!enableValidationLayer) return; 

	VkDebugUtilsMessengerCreateInfoEXT createDebugInfo{}; 
	createDebugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT; 
	createDebugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; 
	createDebugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT; 
	createDebugInfo.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(debugCallback);
	createDebugInfo.pUserData = nullptr;	//Optional

	if (CreateDebugUtilsMessengerEXT(instance, &createDebugInfo, nullptr, &debugMessenger) != VK_SUCCESS) throw std::runtime_error("Failed to set up debug Messenger"); 
}

//Check Functions

bool HelloTriangleApp::validExtensionsSupport(std::vector<const char*> RequiredExtensions, std::vector<VkExtensionProperties>& AvailableExtensions) {
	for (const auto& RequierdExtension : RequiredExtensions) {
		for (const auto& AvailableExtension : AvailableExtensions) {
			if (std::string(RequierdExtension) == AvailableExtension.extensionName) break; 
			if (AvailableExtension.extensionName == AvailableExtensions.back().extensionName) return false; 
		}
	}
	return true; 
}

bool HelloTriangleApp::validValidationLayerSupport() {
	uint32_t LayerCount; 
	vkEnumerateInstanceLayerProperties(&LayerCount, nullptr); 

	std::vector<VkLayerProperties> availableLayers(LayerCount); 
	vkEnumerateInstanceLayerProperties(&LayerCount, availableLayers.data()); 

	bool Valid = true; 

	for (const auto layerName : validationLayers) {
		for (const auto &availableLayer : availableLayers) {
			if (availableLayer.layerName == std::string(layerName)) break; 
			if (availableLayer.layerName == availableLayers.back().layerName) Valid = false; 
		}
		if (Valid == false) break; 
	}

	return Valid; 
}

std::vector<const char*> HelloTriangleApp::getRequierdExtensions(){
	uint32_t ExtensionCount = 0; 
	const char** glfwExtensions; 

	glfwExtensions = glfwGetRequiredInstanceExtensions(&ExtensionCount); 

	std::vector<const char*> Extensions(glfwExtensions, glfwExtensions + ExtensionCount); 

	if (enableValidationLayer) Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); 

	return Extensions; 
}

//Debug Message functions 

VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity, VkDebugUtilsMessageTypeFlagBitsEXT MessageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData){
	std::cerr << "Validation layer" << pCallbackData->pMessage << std::endl; 

	return VK_FALSE; 
}

void HelloTriangleApp::mainloop() {
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); 
	}
}


void HelloTriangleApp::cleanup() {
	if (enableValidationLayer) DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	vkDestroyInstance(instance, nullptr); 

	glfwDestroyWindow(window); 
	glfwTerminate(); 
}


int main() {
	HelloTriangleApp app; 

	try {
		app.run(); 
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl; 
		return EXIT_FAILURE; 
	}
	return EXIT_SUCCESS; 
}