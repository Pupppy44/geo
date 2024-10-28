import React from 'react';
import ReactDOM from 'react-dom/client';
import './index.css';
import App from './App';
import { FluentProvider, webDarkTheme } from '@fluentui/react-components';
import { parseGame } from './helpers/GameHandler';

const s = `
<geo>
    <object class="rectangle">
        <property name="name" type="2" value="topbar" />
        <property name="x" type="1" value="0" />
        <property name="y" type="1" value="0" />
        <property name="width" type="1" value="1000" /> <!-- Adjust as needed -->
        <property name="height" type="1" value="50" />
        <property name="color" type="2" value="#262626" />
        <property name="opacity" type="1" value="1" />
    </object>
    <object class="text">
        <property name="name" type="2" value="topbar_title" />
        <property name="x" type="1" value="20" />
        <property name="y" type="1" value="10" />
        <property name="text" type="2" value="My Game" />
        <property name="font" type="2" value="Gilroy" />
        <property name="weight" type="1" value="700" />
        <property name="size" type="1" value="24" />
        <property name="color" type="2" value="#ffffff" />
        <property name="align" type="2" value="left" />
    </object>

    <object class="rectangle">
        <property name="name" type="2" value="hero_section" />
        <property name="x" type="1" value="0" />
        <property name="y" type="1" value="50" />
        <property name="width" type="1" value="1000" /> <!-- Adjust as needed -->
        <property name="height" type="1" value="415" />
        <property name="color" type="2" value="#ff0000" />
        <property name="opacity" type="1" value="1" />
    </object>
    <object class="text">
        <property name="name" type="2" value="hero_title" />
        <property name="x" type="1" value="200" />
        <property name="y" type="1" value="100" />
        <property name="text" type="2" value="Welcome to Word Bomb!" />
        <property name="font" type="2" value="Gilroy" />
        <property name="weight" type="1" value="700" />
        <property name="size" type="1" value="36" />
        <property name="color" type="2" value="#ffffff" />
        <property name="align" type="2" value="center" />
    </object>
    <object class="text">
        <property name="name" type="2" value="hero_subtitle" />
        <property name="x" type="1" value="200" />
        <property name="y" type="1" value="150" />
        <property name="text" type="2" value="Get ready to explode your vocabulary." />
        <property name="font" type="2" value="Gilroy" />
        <property name="weight" type="1" value="400" />
        <property name="size" type="1" value="24" />
        <property name="color" type="2" value="#ffffff" />
        <property name="align" type="2" value="center" />
    </object>

    <object class="rectangle">
        <property name="name" type="2" value="footer" />
        <property name="x" type="1" value="0" />
        <property name="y" type="1" value="465" /> <!-- Adjust based on height -->
        <property name="width" type="1" value="1000" /> <!-- Adjust as needed -->
        <property name="height" type="1" value="50" />
        <property name="color" type="2" value="#262626" />
        <property name="opacity" type="1" value="1" />
    </object>
    <object class="text">
        <property name="name" type="2" value="footer_text" />
        <property name="x" type="1" value="20" />
        <property name="y" type="1" value="470" />
        <property name="text" type="2" value="© 2024 Word Bomb. All rights reserved." />
        <property name="font" type="2" value="Gilroy" />
        <property name="weight" type="1" value="400" />
        <property name="size" type="1" value="16" />
        <property name="color" type="2" value="#ffffff" />
        <property name="align" type="2" value="left" />
    </object>
</geo>`

console.log(parseGame(s));

const root = ReactDOM.createRoot(document.getElementById('root'));

const Main = () => {
  window.web = true;
  return (
    <FluentProvider theme={webDarkTheme}>
      <App />
    </FluentProvider>
  );
};

root.render(<Main />);