import * as React from "react";
import { Title1, Title3 } from "@fluentui/react-components";
import { ImageCard } from "../components/ImageCard";
import { ObjectTree } from '../components/creator/ObjectTree';


const Home = () => {
    return (
        <div style={{
            backgroundColor: '#1E1E1E',
            color: '#D4D4D4',
            padding: '1rem',
            minHeight: 'calc(100vh - 30px)',
        }}>
            <Title1>Home</Title1>
            <br />
            <br />
            <Title3>Recent Games</Title3>
            <br /><br />
            <ImageCard imageSrc="https://cdn.discordapp.com/banners/1216333617094066298/fb25f6d82092d0e1c0a4a4a4d9a2f159.png?size=256" title="Game 1" />

            <ObjectTree />

        </div>
    );
};

export default Home;
