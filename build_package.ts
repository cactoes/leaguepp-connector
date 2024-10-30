import { $ } from "bun";
import * as fs from "fs";
import * as path from "path";
import archiver from "archiver";

await $`mkdir -p build && cd build && cmake .. -G "Visual Studio 17 2022" && cmake --build . --config Release --target connector && cmake --build . --config Debug --target connector`;

const dbg_path = path.join(__dirname, "build", "Debug", "connector.lib");
const rel_path = path.join(__dirname, "build", "Release", "connector.lib");

const archive = archiver("zip", {
    zlib: { level: 9 }
});

archive.pipe(fs.createWriteStream(__dirname + "/league_connector-win-x64.zip"));
archive.append(fs.createReadStream(dbg_path), { name: "lib/connector.debug.lib" });
archive.append(fs.createReadStream(rel_path), { name: "lib/connector.release.lib" });
archive.directory(__dirname + "/src/connector/include/connector", "include/connector");
archive.finalize();