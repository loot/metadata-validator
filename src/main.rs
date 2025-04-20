use std::{path::PathBuf, sync::LazyLock};

use clap::{Parser, command};
use tempfile::TempDir;

static VERSION_STRING: LazyLock<String> = LazyLock::new(version);

#[derive(Debug, Parser)]
#[command(version = &**VERSION_STRING, about, author, disable_version_flag = true)]
struct Cli {
    /// The metadata file to validate.
    metadata_file_path: PathBuf,

    /// The prelude metadata file to substitute in before validation.
    prelude_file_path: Option<PathBuf>,

    /// Print version
    #[arg(short = 'v', long, action = clap::builder::ArgAction::Version)]
    version: (),
}

#[derive(Clone, Copy, Default, Debug, Eq, PartialEq, Ord, PartialOrd, Hash)]
pub struct DatabaseLockPoisonError;

impl std::fmt::Display for DatabaseLockPoisonError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "the database's lock has been poisoned")
    }
}

impl std::error::Error for DatabaseLockPoisonError {}

fn main() -> Result<(), anyhow::Error> {
    let cli = Cli::parse();

    println!(
        "Validating metadata file: {}",
        cli.metadata_file_path.display()
    );
    if let Some(path) = &cli.prelude_file_path {
        println!("Using prelude file: {}", path.display());
    }

    load_metadata(&cli)?;

    println!("SUCCESS!");
    println!();

    Ok(())
}

fn load_metadata(cli: &Cli) -> Result<(), anyhow::Error> {
    let (_tmp_dir, game_path) = create_mock_game_install()?;

    // The choice of game type doesn't matter.
    let game = libloot::Game::new(libloot::GameType::Oblivion, &game_path)?;

    let database = game.database();
    let mut database = database.write().map_err(|_| DatabaseLockPoisonError)?;

    if let Some(path) = &cli.prelude_file_path {
        database.load_masterlist_with_prelude(&cli.metadata_file_path, path)?;
    } else {
        database.load_masterlist(&cli.metadata_file_path)?;
    }

    Ok(())
}

fn create_mock_game_install() -> Result<(TempDir, PathBuf), std::io::Error> {
    let tmp_dir = tempfile::tempdir()?;
    let game_path = tmp_dir.path().join("Oblivion");
    let data_path = game_path.join("Data");

    std::fs::create_dir_all(&data_path)?;
    std::fs::File::create(data_path.join("Oblivion.esm"))?;

    Ok((tmp_dir, game_path))
}

fn version() -> String {
    // libloot from crates.io isn't built with an embedded revision, and it's
    // not worth trying to get that data from its VCS info JSON file, because
    // it can only be one immutable value per version anyway.
    let libloot_revision = libloot::libloot_revision();
    if libloot_revision.is_empty() {
        format!(
            "v{}, build revision {}\nUsing libloot v{}",
            env!("CARGO_PKG_VERSION"),
            build_revision(),
            libloot::libloot_version()
        )
    } else {
        format!(
            "v{}, build revision {}\nUsing libloot v{}, build revision {}",
            env!("CARGO_PKG_VERSION"),
            build_revision(),
            libloot::libloot_version(),
            libloot_revision
        )
    }
}

const fn build_revision() -> &'static str {
    if let Some(s) = option_env!("LMV_REVISION") {
        s
    } else {
        "unknown"
    }
}
