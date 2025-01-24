import React from 'react';
import { ListGroup } from 'react-bootstrap';
import { RiPauseCircleFill, RiPlayCircleFill, RiMore2Line, RiHeartFill, RiHeartLine, RiDeleteBin3Line } from "react-icons/ri";
import { BrowserRouter as Router, Link, NavLink, Route } from 'react-router-dom';
import ReactPlayer from "react-player";

class Song extends React.Component {

  constructor(props) {
    super(props);
    this.state = {
      track: this.props.track,
      user: this.props.user,
      playing: false,
      isFetching: true,
      error: null
    };
    this.handleClickLike = this.handleClickLike.bind(this);
  }

  handleClickLike(event) {
    const user = this.state.user;
    var track = this.state.track;
    fetch("/music/like_track", { method: "post", body: JSON.stringify(track) });
    if (track.like.includes(user)) {
      track.like.splice(track.like.indexOf(user), 1)
    } else {
      track.like.push(user);
    }
    this.setState({
      track: track,
    })
    event.preventDefault();
  }

  render() {

    const track = this.state.track;
    const user = this.state.user;
    return (
      <>
        <ListGroup className="d-flex" horizontal>
          <ListGroup.Item className="col p-2 border-0" >
            {track["name"]}
            <div>
              <ReactPlayer
                // url={`/mediafiles/${track["author"]} - ${track["name"]}.mp3`}
                url={"https://www.dropbox.com/scl/fi/q4z5ivywefkpsag94xj9i/System-Of-A-Down-Mr.-Jack.mp3?rlkey=2pvx6uf7s122vbdaje5b3ljr9&st=p941vvs2&dl=0"}
                width="100%"
                height="100%"
                playing={false}
                controls={true}
              />
            </div>
          </ListGroup.Item>
          <ListGroup.Item className="mr-auto p-2 col border-0">
            <Link className="text-success" to={`/authors/${track["author"]}`} >{track["author"]}</Link>
          </ListGroup.Item>
          {
            track["like"].includes(user)
              ? <ListGroup.Item className="p-2 border-0"><RiHeartFill onClick={this.handleClickLike} /></ListGroup.Item>
              : <ListGroup.Item className="p-2 border-0"><RiHeartLine onClick={this.handleClickLike} /></ListGroup.Item>
          }
          <ListGroup.Item className="p-2 border-0"><RiDeleteBin3Line /></ListGroup.Item>
          <ListGroup.Item className="p-2 border-0"><RiMore2Line /></ListGroup.Item>

        </ListGroup>
      </>
    )
  }
}

export default Song;