import express from "express";
import cors from "cors";
import multer from "multer";
import path from "path";
import { fileURLToPath } from "url";
import fs from "fs";

const app = express();
const port = 3000;
const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const uploadDir = path.join(__dirname, "uploads");

// 서버 시작 시 업로드 폴더 생성
fs.mkdirSync(uploadDir, { recursive: true });

// CORS 설정
// http://localhost:8080에서 실행되는 별도의 클라이언트가
// http://localhost:3000의 API를 호출할 수 있도록 허용
app.use(
  cors({
    origin: "http://localhost:8080",
    allowedHeaders: ["Content-Type", "Authorization"],
  }),
);

// JSON POST 요청 처리 설정
// 요청의 Content-Type이 application/json 일 때 동작함
app.use(express.json());

// 업로드된 파일 저장 위치 설정
// 기본적으로 파일명은 무작위로 생성되지만, 여기서는 파일이름에 타임스탬프를 붙여 고유하게 만듦
const storage = multer.diskStorage({
  destination: (req, file, cb) => {
    cb(null, uploadDir);
  },
  filename: (req, file, cb) => {
    const ext = path.extname(file.originalname);
    const base = path.basename(file.originalname, ext);
    cb(null, `${base}-${Date.now()}${ext}`);
  },
});
const upload = multer({ storage });

// Home
app.get("/", (req, res) => {
  res.sendFile(path.join(__dirname, "index.html"));
});

// API 생성
// 1. GET 요청
app.get("/hello", (req, res) => {
  res.json({
    message: "Hello from Node.js server",
  });
});

// 2. 인증 헤더가 포함된 GET
app.get("/auth", (req, res) => {
  const authorization = req.get("Authorization");

  // 인증 헤더에 Authorization이 없으면 오류 발생
  if (!authorization) {
    return res.status(401).json({
      error: "Authorization header is required",
    });
  }

  res.json({
    message: "Authorization header received",
    authorization: authorization,
  });
});

// 3. JSON body를 가지고 POST 요청
app.post("/echo", (req, res) => {
  console.log("POST body:");
  console.log(req.body);

  // 요청으로 받은 JSON body를 그대로 리턴
  res.json({
    received: req.body,
  });
});

// 4. 파일 업로드
app.post("/upload", upload.single("uploadFile"), (req, res) => {
  if (!req.file) {
    return res.status(400).json({
      error: "No file uploaded",
    });
  }

  res.json({
    message: "File uploaded successfully",
    originalName: req.file.originalname,
    filename: req.file.filename,
    size: req.file.size,
    downloadUrl: `/download/${encodeURIComponent(req.file.filename)}`,
  });
});

// 5. 파일 다운로드
app.get("/download/:filename", (req, res) => {
  // root 옵션을 지정하면 Express가 파일 경로를 uploads 폴더 내부로 제한함
  res.sendFile(req.params.filename, { root: uploadDir });
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});
