import fs from "fs";
import path from "path";

const __dirname = path.resolve(path.dirname(""));
const PROJECT_ROOT = path.join(__dirname, "../..");

const INTERFACE_QRC_PATH = fs.writeFileSync(
  path.join(PROJECT_ROOT, "interface.qrc"),
  (() => {
    function recursiveScan(root) {
      let result = `\t\t<file>${path.relative(PROJECT_ROOT, root)}</file>\n`;
      console.log(path.relative(PROJECT_ROOT, root));
      fs.readdirSync(root).forEach((file) => {
        const name = path.resolve(root, file);
        const stat = fs.statSync(name);
        if (stat && stat.isDirectory()) {
          result += recursiveScan(name);
        }
      });
      return result;
    }

    return `<RCC>
    <qresource prefix="/">
${recursiveScan(
      path.join(__dirname, "build")
    )}\t</qresource>
</RCC>`;
  })()
);
