const hostName = window.location.hostname.toLowerCase();

const basicLink = process.env.NODE_ENV === "development" ? "http://localhost:8000/" : "/"

const devLinks = {
    functionResolver : basicLink + "api/FunctionResolver",
}




const links = process.env.NODE_ENV === "development" ? devLinks : devLinks;

export default links