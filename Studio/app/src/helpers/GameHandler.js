// Description: Parse and process game files for the Geo Studio game engine.

// Parse a game file
export const parseGame = (xmlString) => {
    // Create a new DOMParser instance
    const parser = new DOMParser();
    // Parse the XML string into a document
    const xmlDoc = parser.parseFromString(xmlString, 'application/xml');
    
    // Check for parsing errors
    const parserError = xmlDoc.getElementsByTagName('parsererror');
    if (parserError.length > 0) {
        console.error('Error parsing XML:', parserError[0].textContent);
        return null;
    }

    // Get the root node (<geo>)
    const root = xmlDoc.getElementsByTagName('geo')[0];
    if (!root) {
        console.error('No <geo> element found in the XML.');
        return null;
    }

    // Create the game object
    const game = {
        name: root.getAttribute('name') || 'Untitled Game',
        description: root.getAttribute('description') || '',
        thumbnail: root.getAttribute('thumbnail') || 'https://example.com',
        id: root.getAttribute('id') || '-1',
        objects: []
    };

    // Loop through all objects in <geo>
    const objectNodes = root.getElementsByTagName('object');
    for (let i = 0; i < objectNodes.length; i++) {
        const objectNode = objectNodes[i];

        // Get object type
        const type = objectNode.getAttribute('class') || '';
        if (type === '') continue;

        // Parse properties
        const properties = parseProperties(objectNode);

        // Create object structure
        const gameObject = {
            type,
            properties
        };

        // Add to the objects array
        game.objects.push(gameObject);
    }

    return game;
}

const parseProperties = (objectNode) => {
    const properties = [];
    const propertyNodes = objectNode.getElementsByTagName('property');

    for (let i = 0; i < propertyNodes.length; i++) {
        const propertyNode = propertyNodes[i];
        const name = propertyNode.getAttribute('name');
        const type = propertyNode.getAttribute('type'); // assuming this is a string representation
        const value = propertyNode.getAttribute('value');

        properties.push({
            name: name,
            type: type,
            value: value
        });
    }

    return properties;
}


// Convert a game object to XML (for saving edited games)
export const gameToXML = (game) => {
    // Create a new XML document
    const xmlDoc = document.implementation.createDocument('', '', null);

    // Create the root <geo> element
    const geoElement = xmlDoc.createElement('geo');
    geoElement.setAttribute('name', game.name);
    geoElement.setAttribute('description', game.description);
    geoElement.setAttribute('thumbnail', game.thumbnail);
    geoElement.setAttribute('id', game.id);
    xmlDoc.appendChild(geoElement);

    // Loop through the objects and create <object> elements
    game.objects.forEach(obj => {
        const objectElement = xmlDoc.createElement('object');
        objectElement.setAttribute('class', obj.type);

        // Loop through the properties and create <property> elements
        obj.properties.forEach(prop => {
            const propertyElement = xmlDoc.createElement('property');
            propertyElement.setAttribute('name', prop.name);
            propertyElement.setAttribute('type', prop.type);
            propertyElement.setAttribute('value', prop.value);
            objectElement.appendChild(propertyElement);
        });

        // Append the object element to the geo element
        geoElement.appendChild(objectElement);
    });

    // Serialize the XML document to a string
    const serializer = new XMLSerializer();
    return serializer.serializeToString(xmlDoc);
}