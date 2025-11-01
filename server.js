// server.js

// Import required modules
const http = require('http');
const { MongoClient } = require('mongodb');

// --- 1. CONFIGURATION ---

// ⚠️ REPLACE THIS WITH YOUR FULL CONNECTION STRING 
// (e.g., 'mongodb+srv://user:password@cluster0.abcde.mongodb.net/?retryWrites=true&w=majority')
const uri = "mongodb+srv://<YOUR_USERNAME>:<YOUR_PASSWORD>@cluster0.abcde.mongodb.net/?retryWrites=true&w=majority"; 

const client = new MongoClient(uri);
const hostname = '0.0.0.0'; // Listen on all available network interfaces (important for ESP32)
const port = 3000;          // The port your server will run on
const DB_NAME = "IOT_Timer_DB";
const COLLECTION_NAME = "TimerData";


// --- 2. CONNECTION FUNCTION ---

async function run() {
  try {
    // Connect to the MongoDB Atlas cluster
    await client.connect();
    console.log("✅ Successfully connected to MongoDB Atlas!");
    
    // Get references to the database and collection
    const db = client.db(DB_NAME);
    const collection = db.collection(COLLECTION_NAME);
    
    // Start the HTTP server after the database connection is established
    startHttpServer(collection);

  } catch (error) {
    console.error("❌ Failed to connect to MongoDB Atlas:", error);
    process.exit(1); // Exit if connection fails
  }
}

// --- 3. HTTP SERVER FUNCTION ---

function startHttpServer(collection) {
  const server = http.createServer((req, res) => {
    
    // Only handle POST requests to the '/log-timer' path
    if (req.method === 'POST' && req.url === '/log-timer') {
      let body = '';
      
      // Accumulate the data chunk by chunk
      req.on('data', (chunk) => {
        body += chunk.toString();
      });
      
      // When all data is received
      req.on('end', async () => {
        try {
          // Attempt to parse the incoming JSON data from the ESP32
          const data = JSON.parse(body);
          console.log(`📡 Received data: ${JSON.stringify(data)}`);
          
          // Create the document to insert, adding a server timestamp
          const documentToInsert = {
            ...data,
            timestamp: new Date()
          };

          // Insert the document into MongoDB
          await collection.insertOne(documentToInsert);
          console.log(`💾 Data logged successfully to ${COLLECTION_NAME}`);

          // Send a success response back to the ESP32
          res.statusCode = 200;
          res.setHeader('Content-Type', 'text/plain');
          res.end('Data Received and Logged!');

        } catch (e) {
          console.error("❌ Error processing request or inserting data:", e);
          // Send an error response
          res.statusCode = 500;
          res.setHeader('Content-Type', 'text/plain');
          res.end('Server Error');
        }
      });
      
    } else {
      // For all other requests (GET, other paths), send a simple response
      res.statusCode = 404;
      res.setHeader('Content-Type', 'text/plain');
      res.end('Endpoint Not Found. Use POST to /log-timer');
    }
  });

  server.listen(port, hostname, () => {
    console.log(`🌐 Server running at http://${hostname}:${port}/`);
    console.log(`Your ESP32 should POST to: http://<Your_Local_IP>:${port}/log-timer`);
  });
}

// Start the whole process
run();