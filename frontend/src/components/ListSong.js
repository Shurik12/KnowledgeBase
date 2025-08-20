import React from "react";
import { ListGroup } from 'react-bootstrap';
import Song from './Song';

export const ListSong = ({ data, onLike, onDelete }) => {
  if (!data.tracks || data.tracks.length === 0) {
    return (
      <div className="text-center p-4">
        <p>No tracks found</p>
      </div>
    );
  }

  return (
    <ListGroup variant="flush">
      {data.tracks.map(track => (
        <Song
          key={`track-${track.id}`}
          track={track}
          user={data.user}
          onLike={onLike}
          onDelete={onDelete}
        />
      ))}
    </ListGroup>
  );
};