// Description: Message hook used to send messages between the webview and the C++ side.

const messages = {};

// Dummy for the Chrome webview object
if (!window.chrome || !window.chrome.webview) {
    window.web = true;
    window.chrome = window.chrome || {};
    window.chrome.webview = {
        addEventListener: () => { },
        postMessage: () => {
            const message = "This feature is only available for the desktop version of Geo Studio.";
            alert(message);
        }
    }
}

// Listen for web messages
window.chrome.webview.addEventListener("message", function (event) {
    if (!event.data?.id || !event.data?.data) return;

    // Resolve the corresponding promise immediately
    const { id, data } = event.data;
    if (messages[id]) {
        messages[id].resolve(data);
        delete messages[id]; // Clean up after resolving
    }
});

// Send a message to the C++ side and wait for a response
export const sendMessage = async (name, data) => {
    return new Promise((resolve) => {
        const id = Math.random().toString(36).substring(7);
        messages[id] = { resolve }; // Store the resolve function

        window.chrome.webview.postMessage({
            id,
            name,
            data
        });
    });
}