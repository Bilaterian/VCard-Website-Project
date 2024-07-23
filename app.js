'use strict'

// C library API
const ffi = require('ffi-napi');
const ref = require('ref-napi');
// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
	if(!req.files) {
		return res.status(400).send('No files were uploaded.');
	}
	let uploadFile = req.files.uploadFile;


	// Use the mv() method to place the file somewhere on your server
	uploadFile.mv('uploads/' + uploadFile.name, function(err) {
		if(err) {
		return res.status(500).send(err);
		}

		res.redirect('/');
	});
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.stuff + " " + req.query.junk;
  res.send({
    stuff: retStr
  });
});


app.listen(portNum);
console.log('Running app at localhost: ' + portNum);

app.get('/uploadNew', function(req, res){
	let CardString = req.query.Dcard;
	let filename = req.query.filename;
	let card = libvcparser.JSONtoCard(CardString);
	console.log(card);
	console.log(filename);
	let errorNum = libvcparser.writeCard(filename, card);
	console.log(errorNum);
	
	//uploadFile.mv('uploads/' + filename, function(err) {
	//	if(err) {
	//	return res.status(500).send(err);
	//	}

	//	res.redirect('/');
	//});
});

app.get('/library', function(req, res){
	let Card = libvcparser.getEmptyCard();
	let filepath = __dirname+'/uploads/'+req.query.Dfilename;
	let errorNum = libvcparser.createCard(filepath, Card);

	if(errorNum != 0){
		res.send({
			Dfilename: "null",
			Dname: "null",
			Dprops: 0
		});
	}
	else{
		res.send({
			Dfilename: req.query.Dfilename,
			Dname: libvcparser.getFN(Card),
			Dprops: libvcparser.getNumOfOptionalProperties(Card),
		});
	}
});

app.get('/libraryTest', function(req, res){
	console.log(req.query.Dfilename);
	res.send({
		Dfilename: req.query.Dfilename
	});
});

app.get('/view', function(req, res){
	let Card = libvcparser.getEmptyCard();
	let filepath = __dirname+'/uploads/'+req.query.Dfilename;
	let errorNum = libvcparser.createCard(filepath, Card);
	let CardJSON = libvcparser.CardtoJSON(Card);
	
	if(errorNum == 0){
		res.send({
			Dfilename: CardJSON
		});
	}
});

let CardPtr = ref.types.void;
let CardPtrPtr = ref.refType(CardPtr);
let PropertyPtr = ref.types.void;
let DateTimePtr = ref.types.void;
let ListPtr = ref.types.void;

let libvcparser = ffi.Library(__dirname+'/libvcparser', {
	'addProperty':['void',[ CardPtr, PropertyPtr]],
	'JSONtoCard':[CardPtrPtr,['string']],
	'JSONtoDT':[DateTimePtr,['string']],
	'dtToJSON':['string', [DateTimePtr]],
	'JSONtoProp':[PropertyPtr, ['string']],
	'propToJSON':['string', [PropertyPtr]],
	'JSONtoStrList':[ListPtr, ['string']],
	'strListToJSON':['string', [ListPtr]],
	'validateCard':['int',[CardPtr]],
	'writeCard':['int', ['string',CardPtrPtr]],
	'createCard':['int', ['string',CardPtrPtr]],
	'getEmptyCard':[CardPtrPtr,[]],
	'getCard':[CardPtr,[CardPtrPtr]],
	'getNumOfOptionalProperties':['int', [CardPtrPtr]],
	'getFN':['string',[CardPtrPtr]],
	'CardtoJSON':['string',[CardPtrPtr]],
});
