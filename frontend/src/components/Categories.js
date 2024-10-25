import React, { Component } from "react";
import { render } from "react-dom";
import { ListGroup } from 'react-bootstrap';
import { RiPlayCircleFill, RiMore2Line, RiHeartFill, RiHeartLine, RiDeleteBin3Line } from "react-icons/ri";
import { BrowserRouter as Router, Link, NavLink, Route, useParams } from 'react-router-dom';

import Category from './Category';

class Categories extends React.Component {

	constructor(props) {
		super(props);
		this.state = {
			data: {},
			isFetching: true, 
			error: null 
		};
	}

	componentDidMount() {
		fetch("music/categories")
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

		const categories = this.state.data["categories"];
    	const user = this.state.data["user"];
		return (
				<div className="Categories">
					<ListGroup className="d-flex" style={{ width: '60%' }}>
						<h3 className="text-center">Categories</h3>
						{ categories.map(category => {
							return (
								<ListGroup className="d-flex" horizontal>
									<ListGroup.Item className="p-2 border-0">
									<RiPlayCircleFill/>
									</ListGroup.Item>
									<ListGroup.Item className="col p-2 border-0" >
										<Link className="text-success" to={`/category/${ category["name"]}`} >{ category["name"] } </Link>
									</ListGroup.Item>
									<ListGroup.Item className="p-2 border-0"><RiMore2Line/></ListGroup.Item>

								</ListGroup>
						);
						})}
					</ListGroup>
				</div>
		)
	}
}

export default Categories;