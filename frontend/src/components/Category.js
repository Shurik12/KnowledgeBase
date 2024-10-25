import React, { Component } from "react";
import { render } from "react-dom";
import { BrowserRouter as Router, Link, NavLink, Route, useParams } from 'react-router-dom';

import { ListSong } from './ListSong';

class Category extends React.Component {

	constructor(props) {
		super(props);
		this.state = {
			category: this.props.match.params.category,
			data: {},
			isFetching: true, 
			error: null 
		};
	}

	componentDidMount() {
		const category = this.state.category;
		fetch(`/music/categories/${category}`)
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

		const category = this.state.category;

		return ( 
			<div className="Category">
				<h3 className="text-center" style={{ width: '60%' }}>{ category }</h3>
				<ListSong 
					data={ this.state.data }
				/>
			</div>
		)
	}
}

export default Category;