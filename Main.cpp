#define GLFW_INCLUDE_VULKAN 
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <algorithm>
#include <optional>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <limits>
#include <map>
#include <set>

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateDebugInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"); 
	if (func != nullptr) return func(instance, pCreateDebugInfo, pAllocator, pDebugMessenger);
	else return VK_ERROR_EXTENSION_NOT_PRESENT; 
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"); 
	if (func != nullptr) func(instance, debugMessenger, pAllocator); 
}

struct QueueFamilyIndices;

class HelloTriangleApp {

private: 

	//GLFW Variables

	GLFWwindow* window; 

	const uint32_t WIDTH = 800; 
	const uint32_t HEIGHT = 600; 

	//Vk Variables

	VkInstance instance; 

		//Debug
		VkDebugUtilsMessengerEXT debugMessenger; 
			//Vk Validation Layers 
			const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" }; 
			#ifdef  NDEBUG
				const bool enabelValidationLayer = true; 
			#else
				const bool enableValidationLayer = true; 
			#endif //  NDEBUG

		//Phiyiscal Device
		VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE; 

		const std::vector<const char*> deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME }; 

		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsFamily; 
			std::optional<uint32_t> presentationFamily; 

			bool isComplete() { return graphicsFamily.has_value() && presentationFamily.has_value();  }
		};

		//Window Variables 
		VkSurfaceKHR surface; 
		VkQueue presentQueue; 

		//Logical device Variables 
		VkDevice device; 
	
			//Device Queues
			VkQueue graphicsQueue;

		//Swap Chain 
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilites; 
			std::vector<VkSurfaceFormatKHR> formats; 
			std::vector<VkPresentModeKHR> presentModes; 

			bool isChainAdequate() {
				return !formats.empty() && !presentModes.empty(); 
			}
		};


private: 
	//GLFW functions
	void initWindow(); 

	//Vk functions
	void initVulkan();
	void setupDebugMessenger(); 

		//Window Functions 
		void createSurface(); 

		//Physical Device Functions	
		void pickPhysicalDevice(); 
		int ratePhysicalDevice(VkPhysicalDevice device); 
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
			QueueFamilyIndices indices; 

			uint32_t queueFamilyCount = 0; 
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr); 
			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount); 
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data()); 

			int it = 0; 

			for (const auto& queueFamily : queueFamilies) {
				
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					indices.graphicsFamily = it; 
				}

				VkBool32 presentSupport = false; 
				vkGetPhysicalDeviceSurfaceSupportKHR(device, it, surface, &presentSupport); 

				if (presentSupport) indices.presentationFamily = it; 

				if (indices.isComplete()) break; 
				it++; 
			}

			return indices; 
		}

		//SwapChain functions 
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
			SwapChainSupportDetails details; 

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilites); 

			uint32_t formatCount; 
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

			if (formatCount != 0) {
				details.formats.resize(formatCount); 
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data()); 
			}

			uint32_t presentModeCount; 
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr); 

			if (presentModeCount != 0) {
				details.presentModes.resize(presentModeCount); 
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data()); 
			}

			return details; 
		}

		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats); 
		VkPresentModeKHR choosePresnetMode(const std::vector<VkPresentModeKHR> availableModes); 
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites); 



	//Struct Creation functions
	void createInstance(); 
	void createInfo(VkApplicationInfo& appInfo); 
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createDebugInfo); 
	void createLogicalDevice(); 

	//Check functions
	std::vector<const char*> getRequierdExtensions(); 
	bool validExtensionsSupport(std::vector<const char*> RequiredExtensions, std::vector<VkExtensionProperties>& extensions);
	bool validValidationLayerSupport(); 
	bool isDeviceSuitable(VkPhysicalDevice device); 
	bool checkDeviceExtensionsSupport(VkPhysicalDevice& device); 


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
	createSurface(); 
	pickPhysicalDevice();
	createLogicalDevice(); 
}

void HelloTriangleApp::setupDebugMessenger() {
	if (!enableValidationLayer) return; 

	VkDebugUtilsMessengerCreateInfoEXT createDebugInfo{}; 
	populateDebugMessengerCreateInfo(createDebugInfo); 

	if (CreateDebugUtilsMessengerEXT(instance, &createDebugInfo, nullptr, &debugMessenger) != VK_SUCCESS) throw std::runtime_error("Failed to set up debug Messenger"); 
}

//Window Functions 
void HelloTriangleApp::createSurface(){
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) throw std::runtime_error("Failed to create window surface!"); 
}

//Physical Device Functions 

void HelloTriangleApp::pickPhysicalDevice() {
	uint32_t deviceCount = 0; 
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr); 

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support"); 
	}

	std::vector <VkPhysicalDevice> devices(deviceCount); 
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()); 

	std::multimap<int, VkPhysicalDevice> candidates; 

	for (const auto& device : devices) {
		candidates.insert(std::make_pair(ratePhysicalDevice(device), device)); 
	}

	for (const auto& currentCandidate : candidates) {
		if (currentCandidate.first > 0) PhysicalDevice = currentCandidate.second; 
	}

	if (PhysicalDevice == VK_NULL_HANDLE) throw std::runtime_error("failed to find a suitable GPU!"); 

}

int HelloTriangleApp::ratePhysicalDevice(VkPhysicalDevice device) {
	int score = 0; 

	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;

	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	QueueFamilyIndices indices = findQueueFamilies(device); 

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000; 

	score += deviceProperties.limits.maxImageDimension2D; 

	if (!deviceFeatures.geometryShader) score = 0; 

	return score; 
}

//SwapChain functions 
VkSurfaceFormatKHR HelloTriangleApp::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) return availableFormat; 
	}
	return availableFormats[0]; 
}

VkPresentModeKHR HelloTriangleApp::choosePresnetMode(const std::vector<VkPresentModeKHR> availableModes){
	for (const auto& availableMode : availableModes) {
		if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR) return availableMode; 
	}

	return VK_PRESENT_MODE_FIFO_KHR; 
}

VkExtent2D HelloTriangleApp::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites){
	if (capabilites.currentExtent.width != std::numeric_limits<uint32_t>::max()) return capabilites.currentExtent; 
	else {
		int width, height; 
		glfwGetFramebufferSize(window, &width, &height); 
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height) 
		};
		actualExtent.width = std::clamp(actualExtent.width, capabilites.minImageExtent.width, capabilites.maxImageExtent.width); 
		actualExtent.height = std::clamp(actualExtent.height, capabilites.minImageExtent.height, capabilites.maxImageExtent.height); 
		return actualExtent; 
	}
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
		std::cout << "\n"; 

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	if (enableValidationLayer) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo); 
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo; 
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr; 
	}

	if (!validExtensionsSupport(RequiredExtensions, extensions)) throw std::runtime_error("Invalid Extensions"); 

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) throw std::runtime_error("failed to create Instance");
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); 
}

void HelloTriangleApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createDebugInfo) {
	createDebugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createDebugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createDebugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createDebugInfo.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(debugCallback);
	createDebugInfo.pUserData = nullptr;	//Optional
}

void HelloTriangleApp::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(PhysicalDevice);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos; 
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentationFamily.value() }; 
	
	float QueuePriority = 1.0f; 
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreatInfo{}; 
		queueCreatInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO; 
		queueCreatInfo.queueFamilyIndex = queueFamily; 
		queueCreatInfo.queueCount = 1; 
		queueCreatInfo.pQueuePriorities = &QueuePriority; 
		queueCreateInfos.push_back(queueCreatInfo); 
	}
	

	VkPhysicalDeviceFeatures deviceFeatures{}; 
	//vkGetPhysicalDeviceFeatures(PhysicalDevice, &deviceFeatures); 

	VkDeviceCreateInfo createInfo{}; 
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO; 
	createInfo.pQueueCreateInfos = queueCreateInfos.data(); 
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()); 

	createInfo.pEnabledFeatures = &deviceFeatures; 

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data(); 

	if (enableValidationLayer) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else createInfo.enabledLayerCount = 0; 

	if (vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) throw std::runtime_error("Failed to create logical device!"); 

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue); 
	vkGetDeviceQueue(device, indices.presentationFamily.value(), 0, &presentQueue); 
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

bool HelloTriangleApp::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indicies = findQueueFamilies(device); 

	bool deviceExtensionSupported = checkDeviceExtensionsSupport(device); 

	bool swapChainAdequate = false;
	if (deviceExtensionSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device); 
		swapChainAdequate = swapChainSupport.isChainAdequate(); 
	}
	return indicies.isComplete() && deviceExtensionSupported && swapChainAdequate; 
}

bool HelloTriangleApp::checkDeviceExtensionsSupport(VkPhysicalDevice& device){
	uint32_t extensionCount = 0; 
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr); 

	std::vector<VkExtensionProperties> extensions{ extensionCount }; 
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data()); 

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end()); 

	for (const auto& extension : extensions) {
		requiredExtensions.erase(extension.extensionName); 
	}

	return requiredExtensions.empty();
}

std::vector<const char*> HelloTriangleApp::getRequierdExtensions() {
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

//Main loop 

void HelloTriangleApp::mainloop() {
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); 
	}
}

//Cleanup 

void HelloTriangleApp::cleanup() {

	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr); 
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