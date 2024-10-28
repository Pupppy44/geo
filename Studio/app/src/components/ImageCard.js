import * as React from 'react';
import { Caption1, Text } from '@fluentui/react-components';
import { Card, CardHeader, CardPreview } from '@fluentui/react-components';

export const ImageCard = ({ imageSrc, title, lastEdited }) => {
  const [selected, setSelected] = React.useState(false);

  const onSelectionChange = (_, state) => {
    setSelected(state.selected);
  };

  return (
    <Card
      style={{ width: '400px', maxWidth: '100%', height: 'fit-content' }}
      selected={selected}
      onSelectionChange={onSelectionChange}
    >
      <CardPreview>
        <img src={imageSrc} alt={title} />
      </CardPreview>
      <CardHeader
        header={<Text weight="semibold">{title}</Text>}
        description={<Caption1>Last visited {lastEdited || "never"}</Caption1>}
      />
    </Card>
  );
};
