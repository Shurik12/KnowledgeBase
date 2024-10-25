import React, { Component } from "react";
import { render } from "react-dom";
import { BrowserRouter as Router, Link, NavLink, Route, useParams } from 'react-router-dom';
import { RiPlayCircleFill, RiMore2Line, RiHeartFill, RiHeartLine, RiDeleteBin3Line } from "react-icons/ri";
import { ListGroup, Button } from 'react-bootstrap';
import { ListSong } from './ListSong';
import ReactPlayer from "react-player";

class Author extends React.Component {

	constructor(props) {
		super(props);
		this.state = {
			authorName: this.props.match.params.author,
			author: {},
			username: "",
			isFetching: true, 
			error: null 
		};
		this.handleClickLike = this.handleClickLike.bind(this);
	}

	handleClickLike(event) {
    const username = this.state.username;
    var author = this.state.author;
    fetch("/music/like_author", {method: "post", body: JSON.stringify(author)});
    if (author["likes"].includes(username)) {
      author["likes"].splice(author["likes"].indexOf(username), 1)
    } else {
      author["likes"].push(username);
    }
    this.setState({
      author: author,
    })
    event.preventDefault();
  }

	componentDidMount() {
		const author = this.state.authorName;
		fetch(`/music/authors/${author}`)
			.then(response => response.json())
			.then(result => this.setState({
			username: result["username"],
		  	author: result["author"],
				isFetching: false 
			}))
			.catch(e => {
			  console.log(e);
			  this.setState({
			  	username: result["username"],
			  	author: result["author"],
				isFetching: false,
			  	error: e
			  });
			});
	}

	render() {

		if (this.state.isFetching) return <div>...Loading</div>;
		if (this.state.error) return <div>{`Error: ${e.message}`}</div>;

		const author = this.state.author;
		const username = this.state.username;

		return ( 
			<div className="Author">
				<ListGroup className="d-flex" style={{ width: '60%' }}>
					<h3 className="text-center"> { author["name"] } ({ author["year"] })</h3>
					{ author["tracks"].map(track => {
						return (
							// <ListGroup className="d-flex" horizontal>
							// 	<ListGroup.Item className="p-2 border-0">
							// 	<RiPlayCircleFill/>
							// 	</ListGroup.Item>
							// 	<ListGroup.Item className="col p-2 border-0" >{ track }</ListGroup.Item>
							// 	<ListGroup.Item className="mr-auto p-2 col border-0">{ author["name"] }</ListGroup.Item>
							// 	<ListGroup.Item className="p-2 border-0"><RiDeleteBin3Line/></ListGroup.Item>
							// 	<ListGroup.Item className="p-2 border-0"><RiMore2Line/></ListGroup.Item>
							// </ListGroup>
							<ListGroup className="d-flex" horizontal>
								<ListGroup.Item className="col p-2 border-0" >
									{ track }
									<ReactPlayer
										url = {`/mediafiles/${author["name"]} - ${track}.mp3`}
										width="50%"
										height="50%"
										playing={false}
										controls={true}
									/>
								</ListGroup.Item>
								<ListGroup.Item className="p-2 border-0"><RiDeleteBin3Line/></ListGroup.Item>
								<ListGroup.Item className="p-2 border-0"><RiMore2Line/></ListGroup.Item>
		
							</ListGroup>
						);
					})}
					<div className="d-flex justify-content-end mx-2">
						<span className="badge badge-pill badge-primary" style={{width: '10%'}} >
							{
								author["likes"].includes(username)
								? <RiHeartFill onClick={this.handleClickLike}/>
								: <RiHeartLine onClick={this.handleClickLike}/>
							}
							{ author["likes"].length }
						</span>
					</div>
				</ListGroup>
			</div>
		)
	}
}

export default Author;