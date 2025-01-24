import React from "react";
import { ListGroup } from 'react-bootstrap';

import Song from './Song';

export const ListSong = (data) => {
  return (
    <div style={{ marginBottom: '50px' }}>
      <ListGroup className="d-flex" style={{ width: '60%' }}>
        {data.data["tracks"].map(track => {
          return (
            <>
              <Song
                key={`tack-${track.id}`}
                track={track}
                user={data.data["user"]}
              />
            </>
          );
        })}
      </ListGroup>
    </div>
  )
}
