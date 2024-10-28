// Description: Helper functions for getting data related to games, assets, and more.
import { sendMessage } from './MessageHook';

// Get game by ID
export const getGame = async (id) => {
    if (window.web) {
        // Get games from the browser's local storage
        var games = localStorage.getItem('studio.savedGames');
        if (games) {
            return JSON.parse(games).find(g => g.id === id);
        } else {
            return null;
        }
    } else {
        // Get games from the C++ side
        return await sendMessage("getGame", { id });
    }
}

// Get all games
export const getGames = async () => {
    if (window.web) {
        // Get games from the browser's local storage
        var games = localStorage.getItem('studio.savedGames');
        if (games) {
            return JSON.parse(games);
        } else {
            return [];
        }
    } else {
        // Get games from the C++ side
        return await sendMessage("getGames");
    }
}

// Save a game
// game: { id, data }
export const saveGame = async (game) => {
    if (window.web) {
        // Save games to the browser's local storage
        var games = await getGames();

        // Remove the old game if it exists (game.id)
        games = games.filter(g => g.id !== game.id);

        // Add the new game
        games.push(game);

        localStorage.setItem('studio.savedGames', JSON.stringify(games));
    } else {
        // Save games to the C++ side
        await sendMessage("saveGame", game);
    }
}

// Delete a game
export const deleteGame = async (id) => {
    if (window.web) {
        // Delete games from the browser's local storage
        var games = await getGames();
        games = games.filter(g => g.id !== id);
        localStorage.setItem('studio.savedGames', JSON.stringify(games));
    } else {
        // Delete games from the C++ side
        await sendMessage("deleteGame", id);
    }
}

// Get user information
export const getUser = async () => {
    if (window.web) {
        // Guest user
        return {
            username: "Guest",
            avatar: "https://yt3.ggpht.com/yti/ANjgQV8Apv4PIYsZKKhfB-UmGs587G7AjaDTHDCH_Vb7BwnsWg=s88-c-k-c0x00ffffff-no-rj"
        }
    } else {
        // Get user from the C++ side
        return await sendMessage("getUser");
    }
}