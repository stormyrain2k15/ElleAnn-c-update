# Elle-Ann Repo-Root Frontend (optional dev control surface)

**The product is the Windows service bundle under `../ElleAnn/`.**
This `frontend/` directory is an *optional, dev-time only* web UI used
for exploring the HTTP / WebSocket control surface exposed by
`Elle.Service.HTTP` on the local machine. It is **not installed** with
the service bundle, it does **not** ship to end users, and Elle-Ann
runs to full functionality with this directory entirely absent.

If you do not need a browser-based dev inspector, ignore this folder.

## When you would run it

- You are developing against `/api/*` and want a fast reload loop with
  live WebSocket rendering.
- You want to eyeball the control surface without writing a client.

## When you should NOT run it

- End-user install. The canonical install path is
  `ElleAnn/Deploy/Install.bat` on a Windows PC; no browser involved.
- CI / production. The CRA dev server is not a production web
  server and is not hardened for exposure beyond `localhost`.

## Local run

```bash
cd frontend
yarn install
yarn start       # http://localhost:3000, talks to REACT_APP_BACKEND_URL
```

`REACT_APP_BACKEND_URL` must point at the locally running
`Elle.Service.HTTP` (default `http://localhost:8000`). The frontend
has no persistent state of its own; every view is a projection of
the live C++ services.

## What is NOT here (and why)

- Any production build pipeline. Use `yarn build` if you need a static
  bundle for a dev workstation, but do not deploy the output anywhere
  user-reachable without a real reverse proxy + auth in front of it.
- Any bundled secrets. The frontend reads nothing but
  `REACT_APP_BACKEND_URL` from `.env`.

## If you've seen this file be a stock `create-react-app` README

It was, until the Feb 2026 audit pass (item #116). The original
boilerplate described how to run CRA's tutorial rather than what this
surface actually is for the project, which misled newcomers into
thinking the frontend was part of the product.
