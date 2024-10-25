import React, { Component } from "react";
import { render } from "react-dom";
import { ListGroup } from 'react-bootstrap';

import { ListSong } from './ListSong';

class Profile extends React.Component {

	constructor(props) {
		super(props);
		this.state = {
			username: this.props.match.params.username,
			data: {},
			isFetching: true, 
			error: null 
		};
	}

	componentDidMount() {

		const username = this.state.username;

		fetch(`/music/profiles/${username}`)
			.then(response => response.json())
			.then(result => this.setState({
			  data: result,
				isFetching: false 
			}))
			.catch(e => {
			  console.log(e);
			  this.setState({
			    data: result,
					isFetching: false,
			  	error: e
			  });
			});
	}

	render() {

		if (this.state.isFetching) return <div>...Loading</div>;
		if (this.state.error) return <div>{`Error: ${e.message}`}</div>;

		const user = this.state.username;
		const authors = this.state.data.authors;

		return ( 
			<div className="Profile">
				<ListGroup className="d-flex" style={{ width: '60%' }}>
					<h3 className="text-center"> { user } </h3>
					<h4 className="col p-2 border-0"> Like groups </h4>
					<h6 className="col p-2 border-0"> { authors.join(", ") } </h6>
					<h4 className="col p-2 border-0"> Like music </h4>
					<ListSong 
						data={ this.state.data }
					/>
				</ListGroup>
			</div>
		)
	}
}

export default Profile;