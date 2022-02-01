const hostName = window.location.hostname.toLowerCase();

const basicLink = process.env.NODE_ENV === "development" ? "http://localhost:8000/" : "/"

const devLinks = {
    vision : basicLink + "vision",

    file_receiver : basicLink + "file_receiver",
    extract : basicLink + "extract"
}




const links = process.env.NODE_ENV === "development" ? devLinks : devLinks;

export default links