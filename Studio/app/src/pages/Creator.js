import * as React from 'react';
import { Tree, TreeItem, TabList, Tab } from '@fluentui/react-components';

const Creator = () => {
    return (
        <main style={{ overflowY: 'auto' }}>
            <TabList
                style={{ backgroundColor: '#1e1e1e' }}
                defaultSelectedValue="tab2"
            >
                <Tab value="tab1">First Tab</Tab>
                <Tab value="tab2">Second Tab</Tab>
                <Tab value="tab3">Third Tab</Tab>
                <Tab value="tab4">Fourth Tab</Tab>
            </TabList>

            <div style={{ borderRadius: '4px', border: '1px solid #EDEBE9', padding: '1rem', margin: '0.5rem', minHeight: 'calc(100vh - 30px)' }}>
                <p>Tab 1 Content</p>
            </div>

            <div style={{ borderRadius: '4px', border: '1px solid #EDEBE9', padding: '1rem', margin: '0.5rem', minHeight: 'calc(100vh - 30px)' }}>
                <p>Tab 2 Content</p>
                <Tree>
                    <TreeItem>Item 1</TreeItem>
                    <TreeItem>Item 2</TreeItem>
                    <TreeItem>Item 3</TreeItem>
                </Tree>
            </div>
        </main>
    );
};

export default Creator;
