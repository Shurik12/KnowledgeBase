import React from "react";
import { useParams } from 'react-router';

import { ListSong } from './ListSong';

function withRouter(Component) {
	function ComponentWithRouter(props) {
		let params = useParams()
		return <Component {...props} params={params} />
	}
	return ComponentWithRouter
}

class Category extends React.Component {
	constructor(props) {
		super(props);
		// console.log(this.props.match.params.category);
		this.state = {
			category: this.props.params.category,
			// category: this.props.match.params.category,
			// category: "jazz",
			data: {},
			isFetching: true,
			error: null
		};
	}

	componentDidMount() {
		const category = this.state.category;
		fetch(`/music/category/${category}`)
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
				<h3 className="text-center" style={{ width: '60%' }}>{category}</h3>
				<ListSong
					data={this.state.data}
				/>
			</div>
		)
	}
}

const HOCCategory = withRouter(Category);

export default HOCCategory;