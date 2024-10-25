import React, { Component } from "react";
import { render } from "react-dom";

import { ListSong } from './ListSong';

class AllMusic extends React.Component {

	constructor(props) {
		super(props);
		this.state = {
			data: {},
			isFetching: true, 
			error: null 
		};
	}

	componentDidMount() {
		fetch("/music/shurik_music")
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

		return ( 
			<div className="All music">
				<h3 className="text-center" style={{ width: '60%' }}>All tracks</h3>
				<ListSong 
					data={ this.state.data }
				/>
			</div>
		)
	}
}

export default AllMusic;