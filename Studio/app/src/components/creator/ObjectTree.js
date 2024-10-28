import * as React from "react";
import { Tree, TreeItem, TreeItemLayout } from "@fluentui/react-components";
import { TreeItemPersonaLayout } from "@fluentui/react-components";
import { Image } from "@fluentui/react-components";

export const ObjectTree = () => {
    const [selectedValue, setSelectedValue] = React.useState(null);

    const handleItemSelect = (value) => {
        setSelectedValue(value);
    };

    return (
        <Tree size="small" defaultOpenItems={[]}>
            <TreeItem 
                itemType="branch" 
                value="default-subtree-1"
                onClick={() => handleItemSelect("default-subtree-1")}
            >
                <TreeItemLayout>level 1, item 1</TreeItemLayout>
                <Tree>
                    <TreeItem 
                        itemType="leaf" 
                        selected={selectedValue === "item-1"}
                        onClick={() => handleItemSelect("item-1")}
                    >
                        <TreeItemPersonaLayout
                            media={<Image src="img/logo.png" style={{ width: "28px" }} />}
                        >
                            Imagex
                        </TreeItemPersonaLayout>
                    </TreeItem>
                </Tree>
            </TreeItem>
            <TreeItem 
                itemType="branch" 
                value="default-subtree-2"
                onClick={() => handleItemSelect("default-subtree-2")}
            >
                <TreeItemLayout>level 1, item 2</TreeItemLayout>
                <Tree>
                    <TreeItem 
                        itemType="leaf" 
                        selected={selectedValue === "item-2"}
                        onClick={() => handleItemSelect("item-2")}
                    >
                        <TreeItemPersonaLayout
                            media={<Image src="img/logo.png" style={{ width: "18px" }} />}
                        >
                            Image
                        </TreeItemPersonaLayout>
                    </TreeItem>
                </Tree>
            </TreeItem>
        </Tree>
    );
};
