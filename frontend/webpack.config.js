const path = require("path");
   
module.exports = {
  mode: "development",
  // output:{
  //     path: path.resolve(__dirname, "./public"),     // путь к каталогу выходных файлов - папка public
  //     publicPath: "/public/",
  //     filename: "main.js"       // название создаваемого файла
  // },
  module: {
    rules: [
      {
        test: /\.js$/,
        exclude: /node_modules/,
        use: {
          loader: "babel-loader"
        }
      },
      {
        test: /\.css$/,
        use: ["style-loader", "css-loader"]
      },
      {
        test: /\.(ogg|mp3|wav|mpe?g)$/i,
        use: 'file-loader'
      }
    ]
  }
}