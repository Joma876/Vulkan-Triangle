#define GLFW_INCLUDE_VULKAN 
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <vector>


class HelloTriangleApp {

private: 

	//GLFW Variables

	GLFWwindow* window; 

	const uint32_t WIDTH = 800; 
	const uint32_t HEIGHT = 600; 

	//Vk Variables

	VkInstance instance; 

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

	void createInstance(); 
	void createInfo(VkApplicationInfo& appInfo); 

	std::vector<const char*> getRequierdExtensions(); 
	bool validExtensionsSupport(const char** glfwExtensions, uint32_t size, std::vector<VkExtensionProperties>& extensions);
	bool validValidationLayerSupport(); 

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

	uint32_t glfwExtensionCount = 0; 
	const char** glfwExtensions; 

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); 

	std::cout << "Required extensions\n"; 
	for (uint32_t it = 0; it < glfwExtensionCount; it++) {
		std::cout << "\t" << glfwExtensions[it];
		std::cout << std::endl; 
	}
	std::cout << std::endl; 
	
	uint32_t extensionCount = 0; 
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount); 
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()); 

	std::cout << "available extensions: \n"; 

	for (const auto& extension : extensions) {
		std::cout << "\t" << extension.extensionName << "\n"; 
	}

	createInfo.enabledExtensionCount = glfwExtensionCount; 
	createInfo.ppEnabledExtensionNames = glfwExtensions; 
	
	if (enableValidationLayer) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else createInfo.enabledLayerCount = 0; 

	if (!validExtensionsSupport(glfwExtensions, glfwExtensionCount,extensions)) throw std::runtime_error("Invalid Extensions"); 

	if (vkCreateInstance(&createInfo, nullptr, &instance)) throw std::runtime_error("failed to create Instance");
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); 
}

//Check Functions
bool HelloTriangleApp::validExtensionsSupport(const char** RequiredExtensions, uint32_t size, std::vector<VkExtensionProperties>& AvailableExtensions) {
	bool Valid = true; 
	for (uint32_t it = 0; it < size; it++) {
		for (const auto& extension : AvailableExtensions) {
			if (extension.extensionName == std::string(RequiredExtensions[it])) break;
			if (extension.extensionName == AvailableExtensions.back().extensionName) Valid = false; 
		}
		if (!Valid) break; 
	}
	return Valid; 
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

void HelloTriangleApp::mainloop() {
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); 
	}
}


void HelloTriangleApp::cleanup() {
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