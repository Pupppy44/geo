import * as React from 'react';
import { Avatar, Menu, MenuList, MenuItem, MenuPopover, MenuTrigger, Text } from '@fluentui/react-components';
import { getUser } from '../helpers/DataManager';
import { useState, useEffect } from 'react';

const NavBar = () => {
    // Fetch user information (username and avatar) for the navbar
    const [user, setUser] = useState({});

    useEffect(() => {
        const fetchUser = async () => {
            try {
                const user = await getUser();
                setUser(user);
            } catch (error) {
                console.error(error);
            }
        };

        fetchUser();
    }, []);

    return (
        <>
            <nav style={{
                display: 'flex',
                alignItems: 'center',
                padding: '0.2rem 0.5rem',
                backgroundColor: '#161616',
                color: '#D4D4D4',
                height: '30px'
            }}>

                {/* Logo and App Name */}
                <div style={{ display: 'flex', alignItems: 'center' }}>
                    <img
                        src="img/logo.png"
                        alt="logo"
                        style={{
                            marginLeft: '0.33rem',
                            width: '20px',
                            height: '20px',
                            marginRight: '8px'
                        }}
                    />
                    <Text style={{ userSelect: 'none', color: '#D4D4D4', fontSize: '13px', fontWeight: 500 }}>Geo Studio</Text>
                </div>

                {/* Menu Items */}
                <Menu>
                    <MenuTrigger>
                        <Text style={{ color: '#D4D4D4', fontSize: '13px', marginLeft: '1rem', cursor: 'pointer', userSelect: 'none' }}>File</Text>
                    </MenuTrigger>
                    <MenuPopover>
                        <MenuList>
                            <MenuItem>New Game</MenuItem>
                            <MenuItem>Open Game</MenuItem>
                        </MenuList>
                    </MenuPopover>
                </Menu>
                <Menu>
                    <MenuTrigger>
                        <Text style={{ color: '#D4D4D4', fontSize: '13px', cursor: 'pointer', marginLeft: '1rem', userSelect: 'none' }}>Run</Text>
                    </MenuTrigger>
                    <MenuPopover>
                        <MenuList>
                            <MenuItem>Build Game</MenuItem>
                            <MenuItem>Play Game</MenuItem>
                            <MenuItem>Publish Game</MenuItem>
                        </MenuList>
                    </MenuPopover>
                </Menu>

                {/* Spacer For Dragging Window */}
                <div id="nav-draggable" style={{ flex: 1, paddingTop: "40px", margin: "0 2px 0 2px"}}></div>

                {/* User Information */}
                <Menu>
                    <MenuTrigger className='system-button'>
                        <div style={{
                            marginRight: window.web ? '1rem' : '0.5rem',
                            marginLeft: 'auto',
                            display: 'flex',
                            alignItems: 'center',
                            gap: '0.5rem',
                            cursor: 'pointer'
                        }}>
                            <Avatar
                                image={{
                                    src: user.avatar,
                                    alt: user.username
                                }}
                                size={24}
                            />
                            <Text weight='semibold' style={{ color: '#D4D4D4', fontSize: '13px', userSelect: 'none' }}>{user.username}</Text>
                        </div>
                    </MenuTrigger>
                    <MenuPopover>
                        <MenuList>
                            <MenuItem>Your Profile</MenuItem>
                            <MenuItem>Settings</MenuItem>
                            <MenuItem>Logout</MenuItem>
                        </MenuList>
                    </MenuPopover>
                </Menu>

                {/* Close, Minimize, Maximize Buttons */}
                {window.web && (
                    <div style={{ display: 'flex', gap: '0.5rem' }}>
                        <button id="navbar-minimize" className='system-button' style={{ backgroundColor: 'transparent', border: 'none', cursor: 'pointer' }}>
                            <img src="img/minimize.png" alt="close" style={{ height:"0.5px", width:"12px",marginBottom:'5px' }} />
                        </button>
                        <button id="navbar-maximize" className='system-button' style={{ backgroundColor: 'transparent', border: 'none', cursor: 'pointer' }}>
                            <img src="img/maximize.png" alt="minimize" style={{ width: '12px', height: '12px' }} />
                        </button>
                        <button id="navbar-close" className='system-button' style={{ backgroundColor: 'transparent', border: 'none', cursor: 'pointer' }}>
                            <img src="img/close.png" alt="maximize" style={{ width: '12px', height: '12px' }} />
                        </button>
                    </div>
                )}
            </nav>
        </>
    );
};

export default NavBar;
